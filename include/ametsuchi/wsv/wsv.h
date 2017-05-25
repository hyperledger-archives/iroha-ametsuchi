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

#ifndef WSV_H
#define WSV_H

#include <cpp_redis/redis_client.hpp>
namespace ametsuchi {
namespace wsv {

class WSV {
 public:
  // adds/removes
  virtual bool add_account(uint64_t account_id, std::string name) = 0;
  //  virtual bool add_country(uint64_t account_id, std::string country) = 0;
  virtual bool add_balance(uint64_t account_id, std::uint64_t amount) = 0;
  //  virtual bool remove_balance(uint64_t account_id, std::uint64_t amount) =
  //  0;

  // gets
  virtual std::string get_account_by_id(uint64_t account_id) = 0;
  virtual uint64_t get_balance_by_account_id(uint64_t account_id) = 0;
};

}
}
#endif  // WSV_H
