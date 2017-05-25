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

#ifndef WSV_REDIS_H
#define WSV_REDIS_H

#include <ametsuchi/wsv/wsv.h>
#include <cpp_redis/redis_client.hpp>
namespace ametsuchi {
namespace wsv{

class WSVRedis : public WSV{
 public:
  WSVRedis(std::string host, size_t port) : host_(host), port_(port){
    client_.connect(host_, port_);
  }
  ~WSVRedis();

  bool add_account(uint64_t account_id, std::string name);
  bool add_balance(uint64_t account_id, std::uint64_t amount);


  std::string get_account_by_id(uint64_t account_id);
  uint64_t get_balance_by_account_id(uint64_t account_id);
  void flush_all();


 private:
  cpp_redis::redis_client client_;
  std::string host_;
  size_t port_;
};

}
}

#endif //WSV_REDIS_H
