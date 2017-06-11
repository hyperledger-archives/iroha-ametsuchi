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

#ifndef AMETSUCHI_CLIENT_H
#define AMETSUCHI_CLIENT_H

#include <string>
#include <cstdint>
#include <memory>
#include <storage.grpc.pb.h>

namespace ametsuchi {
class Client {
 public:
  class BlockStream {
   public:
    BlockStream(std::unique_ptr<grpc::ClientReader<iroha::BlockMessage>> reader);
    BlockStream& operator>> (iroha::Block& val);
    operator bool() const;
   private:
    bool state_;
    std::unique_ptr<grpc::ClientReader<iroha::BlockMessage>> reader_;
    iroha::BlockMessage message_;
  };

  Client();
  bool add(iroha::Block *block);
  std::vector<std::string> get_peers();
  bool server_alive(std::chrono::system_clock::time_point deadline);
  iroha::Block get_by_height(uint32_t height);
  iroha::Block get_by_block_hash(std::string hash);
  iroha::Block get_by_transaction_hash(std::string hash);
  bool erase(uint32_t height);
  BlockStream get_range(uint32_t begin, uint32_t end);
 private:
  std::shared_ptr<grpc::Channel> channel_;
  std::unique_ptr<iroha::Storage::Stub> stub_;
};
}

#endif //AMETSUCHI_CLIENT_H
