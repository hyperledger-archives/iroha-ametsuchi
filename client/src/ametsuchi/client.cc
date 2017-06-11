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

#include "ametsuchi/client.h"
#include <grpc++/grpc++.h>

namespace ametsuchi {

using google::protobuf::Empty;

Client::Client() {
  channel_ = grpc::CreateChannel("localhost:50051",
                                 grpc::InsecureChannelCredentials());
  stub_ = iroha::Storage::NewStub(channel_);
}


bool Client::add(iroha::Block *block) {
  grpc::ClientContext context;
  iroha::BlockMessage request;
  request.set_allocated_block(block);
  Empty response;
  auto result = stub_->AddBlock(&context, request, &response).ok();
  request.release_block();
  return result;
}


std::vector<std::string> Client::get_peers() {
  grpc::ClientContext context;
  Empty request;
  iroha::PeersResponse response;
  auto status = stub_->GetPeers(&context, request, &response);
  if (status.ok()) {
    return std::vector<std::string>(
        std::make_move_iterator(response.address().begin()),
        std::make_move_iterator(response.address().end()));
  } else {
    return {};
  }
}


bool Client::server_alive(std::chrono::system_clock::time_point deadline) {
  return channel_->WaitForConnected(deadline);
}


Client::BlockStream Client::get_range(uint32_t begin, uint32_t end) {
  grpc::ClientContext context;
  iroha::RangeGetRequest request;
  request.set_begin(begin);
  request.set_end(end);
  return BlockStream(stub_->GetBlocks(&context, request));
}


iroha::Block Client::get_by_height(uint32_t height) {
  grpc::ClientContext context;
  iroha::BlockIdMessage request;
  request.set_height(height);
  iroha::BlockMessage response;
  auto status = stub_->GetBlock(&context, request, &response);
  auto block = response.block();
  response.release_block();
  if (status.ok()) {
    return block;
  } else {
    return iroha::Block();
  }
}


iroha::Block Client::get_by_block_hash(std::string hash) {
  grpc::ClientContext context;
  iroha::BlockIdMessage request;
  request.set_block_hash(hash);
  iroha::BlockMessage response;
  auto status = stub_->GetBlock(&context, request, &response);
  auto block = response.block();
  response.release_block();
  if (status.ok()) {
    return block;
  } else {
    return iroha::Block();
  }
}


iroha::Block Client::get_by_transaction_hash(std::string hash) {
  grpc::ClientContext context;
  iroha::BlockIdMessage request;
  request.set_transaction_hash(hash);
  iroha::BlockMessage response;
  auto status = stub_->GetBlock(&context, request, &response);
  auto block = response.block();
  response.release_block();
  if (status.ok()) {
    return block;
  } else {
    return iroha::Block();
  }
}


bool Client::erase(uint32_t height) {
  grpc::ClientContext context;
  iroha::BlockIdMessage request;
  request.set_height(height);
  Empty response;
  return stub_->EraseBlock(&context, request, &response).ok();
}


Client::BlockStream::BlockStream(
    std::unique_ptr<grpc::ClientReader<iroha::BlockMessage>> reader)
    : state_(true), reader_(std::move(reader)) {}


Client::BlockStream &Client::BlockStream::operator>>(iroha::Block &val) {
  state_ = reader_->Read(&message_);
  if (!state_) {
    reader_->Finish();
  }
  val = message_.block();
  return *this;
}


Client::BlockStream::operator bool() const { return state_; }
}
