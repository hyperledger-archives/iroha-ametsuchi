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

#ifndef AMETSUCHI_COMMANDSERVICE_H
#define AMETSUCHI_COMMANDSERVICE_H

#include <grpc++/grpc++.h>
#include <command.grpc.pb.h>
#include <block_store_flat.h>

namespace service {

class CommandServiceImpl final : public iroha::Command::Service {
 public:
  CommandServiceImpl(){
    block_store_.reset(new block_store::BlockStoreFlat);
  }
  grpc::Status Append(::grpc::ServerContext *context,
                      const ::iroha::AppendRequest *request,
                      ::iroha::AppendResponse *response) override;
 private:
  std::unique_ptr<block_store::BlockStore> block_store_;
};

}

#endif //AMETSUCHI_COMMANDSERVICE_H
