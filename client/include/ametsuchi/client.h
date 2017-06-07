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
  Client();
  iroha::Block* block();
  uint64_t append();
  std::string get_account_by_id(uint64_t account_id);
  uint64_t get_balance_by_account_id_asset_id(uint64_t account_id,
                                              uint64_t asset_id);
 private:
  std::unique_ptr<iroha::Storage::Stub> stub_;
  std::unique_ptr<iroha::AppendRequest> request_;
};
}

#endif //AMETSUCHI_CLIENT_H
