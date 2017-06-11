/**
 * Copyright Soramitsu Co., Ltd. 2017 All Rights Reserved.
 * http://soramitsu.co.jp
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <hash.h>
#include "storage_service.h"

namespace service {

StorageServiceImpl::StorageServiceImpl() {
  block_store_.reset(new block_store::BlockStoreFlat);
  block_index_.reset(new block_index::BlockIndexRedis);
  transaction_index_.reset(new tx_index::TxIndexRedis);
  auto env = std::getenv("AMWSVBACKEND");
  std::string backend = env ? env : "Postgres";
  wsv_ = wsv::Manager::instance().make_WSV(backend);

  std::function<bool(iroha::Action)> handler = [this](const iroha::Action &action) {
    auto add_account = action.add_account();
    auto signatories = add_account.signature();
    if (signatories.empty() || signatories.size() > UINT8_MAX){
      return false;
    }
    auto account_id = signatories.Get(0).public_key();
    wsv_->add_account(account_id, signatories.size(), 0);
    for (const auto &i : signatories) {
      wsv_->add_signatory(account_id, i.public_key());
    }
    return true;
  };
  handler_.insert(std::make_pair(iroha::Action::kAddAccount, handler));

  handler = [this](const iroha::Action &action) {
    auto register_signatories = action.register_signatories();

    return true;
  };
  handler_.insert(std::make_pair(iroha::Action::kRegisterSignatories, handler));

  handler = [this](const iroha::Action &action) {
    auto add_peer = action.add_peer();
    return true;
  };
  handler_.insert(std::make_pair(iroha::Action::kAddPeer, handler));
}


grpc::Status StorageServiceImpl::GetPeers(::grpc::ServerContext *context,
                                          const ::google::protobuf::Empty *request,
                                          ::iroha::PeersResponse *response) {
  auto peers = wsv_->get_peers();
  for (const auto &i : peers) {
    response->add_address(i);
  }
  return grpc::Status::OK;
}


grpc::Status StorageServiceImpl::AddBlock(::grpc::ServerContext *context,
                                          const ::iroha::BlockMessage *request,
                                          ::google::protobuf::Empty *response) {
  auto block = request->block();
  std::vector<uint8_t> buffer(block.ByteSize());
  block.SerializeToArray(buffer.data(), block.GetCachedSize());
  block_store_->add(block.meta().height(), buffer);

  // Block hash
  std::string hash(32, '\0');
  block.meta().SerializeToArray(buffer.data(), block.meta().ByteSize());
  block.body().SerializeToArray(buffer.data() + block.meta().ByteSize(), block.body().ByteSize());
  // TODO remove cast
  utils::sha3_256((unsigned char *) &hash.at(0), buffer.data(), block.meta().ByteSize() + block.body().ByteSize());
  block_index_->add_blockhash_blockid(hash, block.meta().height());

  for (int i = 0; i < block.body().transaction().size(); ++i){
    auto transaction = block.body().transaction(i);
    // Transaction hash
    transaction.meta().SerializeToArray(buffer.data(), transaction.meta().ByteSize());
    transaction.body().SerializeToArray(buffer.data() + transaction.meta().ByteSize(), transaction.body().ByteSize());
    utils::sha3_256((unsigned char *) &hash.at(0), buffer.data(), transaction.meta().ByteSize() + transaction.body().ByteSize());
    transaction_index_->add_txhash_blockhash_txid(hash, block.meta().height(), i);
    // WSV
    for (const auto &action: transaction.body().action()) {
      auto handler = handler_.at(action.command_case());
    }
  }

  return grpc::Status::OK;
}


grpc::Status StorageServiceImpl::GetBlock(::grpc::ServerContext *context,
                                          const ::iroha::BlockIdMessage *request,
                                          ::iroha::BlockMessage *response) {
  uint32_t height;
  if (request->id_case() == iroha::BlockIdMessage::IdCase::kHeight) {
    height = request->height();
  } else if (request->id_case() == iroha::BlockIdMessage::kBlockHash){
    height = block_index_->get_blockid_by_blockhash(request->block_hash());
  } else if (request->id_case() == iroha::BlockIdMessage::kTransactionHash) {
    auto block_hash = transaction_index_->get_blockhash_by_txhash(request->transaction_hash());
    height = block_index_->get_blockid_by_blockhash(block_hash);
  } else {
    return grpc::Status(grpc::StatusCode::INVALID_ARGUMENT, "Bad request");
  }

  if (height > block_store_->last_id()){
    return grpc::Status(grpc::StatusCode::OUT_OF_RANGE, "Block id greater than last id");
  }

  auto blob = block_store_->get(request->height());
  if (blob.empty()){
    return grpc::Status(grpc::StatusCode::NOT_FOUND, "Block not found");
  }
  auto block = std::make_unique<iroha::Block>();
  auto status = block->ParseFromArray(blob.data(), blob.size());
  if (!status){
    return grpc::Status(grpc::StatusCode::DATA_LOSS, "Block parsing failed");
  }
  response->set_allocated_block(block.get());
  return grpc::Status::OK;
}


grpc::Status StorageServiceImpl::EraseBlock(::grpc::ServerContext *context,
                                            const ::iroha::BlockIdMessage *request,
                                            ::google::protobuf::Empty *response) {
  return grpc::Status(grpc::StatusCode::UNIMPLEMENTED, "Not implemented yet");
}


grpc::Status StorageServiceImpl::GetBlocks(::grpc::ServerContext *context,
                                           const ::iroha::RangeGetRequest *request,
                                           ::grpc::ServerWriter<::iroha::BlockMessage> *writer) {
  if (request->begin() > request->end()){
    return grpc::Status(grpc::StatusCode::INVALID_ARGUMENT, "Invalid range");
  }
  if (request->begin() > block_store_->last_id() || request->end() > block_store_->last_id()){
    return grpc::Status(grpc::StatusCode::OUT_OF_RANGE, "Block id greater than last id");
  }
  auto begin = block_store_->begin() + request->begin(), end = block_store_->begin() + request->end();
  for (; begin != end; ++begin) {
    auto blob = *begin;
    if (blob.empty()){
      return grpc::Status(grpc::StatusCode::NOT_FOUND, "Block not found");
    }
    auto block = std::make_unique<iroha::Block>();
    auto status = block->ParseFromArray(blob.data(), blob.size());
    if (!status){
      return grpc::Status(grpc::StatusCode::DATA_LOSS, "Block parsing failed");
    }
    iroha::BlockMessage message;
    message.set_allocated_block(block.get());
    writer->Write(message);
  }
  return grpc::Status::OK;
}
}
