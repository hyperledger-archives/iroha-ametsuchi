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

#ifndef AMETSUCHI_STORAGE_SERVICE_H
#define AMETSUCHI_STORAGE_SERVICE_H

#include <block_store_flat.h>
#include <block_index_redis.h>
#include <tx_index_redis.h>
#include <manager.h>
#include <storage.grpc.pb.h>
#include <wsv.h>
#include <unordered_map>

namespace service {
class StorageServiceImpl final : public iroha::Storage::Service {
 public:
  StorageServiceImpl();
  grpc::Status AddBlock(::grpc::ServerContext *context,
                        const ::iroha::BlockMessage *request,
                        ::google::protobuf::Empty *response) override;
  grpc::Status GetBlock(::grpc::ServerContext *context,
                        const ::iroha::BlockIdMessage *request,
                        ::iroha::BlockMessage *response) override;
  grpc::Status EraseBlock(::grpc::ServerContext *context,
                          const ::iroha::BlockIdMessage *request,
                          ::google::protobuf::Empty *response) override;
  grpc::Status GetBlocks(::grpc::ServerContext *context,
                         const ::iroha::RangeGetRequest *request,
                         ::grpc::ServerWriter<::iroha::BlockMessage> *writer) override;
  grpc::Status GetPeers(::grpc::ServerContext *context,
                        const ::google::protobuf::Empty *request,
                        ::iroha::PeersResponse *response) override;
 private:
  std::unique_ptr<block_store::BlockStore> block_store_;
  std::unique_ptr<block_index::BlockIndex> block_index_;
  std::unique_ptr<tx_index::TxIndex> transaction_index_;
  std::unique_ptr<wsv::WSV> wsv_;
  std::unordered_map<iroha::Action::CommandCase, std::function<bool(iroha::Action)>> handler_;
};
}

#endif  // AMETSUCHI_STORAGE_SERVICE_H
