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

#ifndef AMETSUCHI_WSV_POSTGRES_H
#define AMETSUCHI_WSV_POSTGRES_H

#include <ametsuchi/wsv/wsv.h>
#include <pqxx/pqxx>

namespace ametsuchi {
namespace wsv {
class WSVPostgres : public WSV {
 public:
  WSVPostgres();
  bool add_balance(uint64_t account_id, std::uint64_t amount) override;
  uint64_t get_balance_by_account_id(uint64_t account_id) override;
  bool add_account(uint64_t account_id, std::string name) override;
  bool add_balance(uint64_t account_id, uint64_t asset_id, std::uint64_t amount);
  bool add_domain(uint64_t domain_id,
                    std::string name,
                    uint64_t root_account_id);
  bool add_asset(uint64_t asset_id, std::string name, uint64_t domain_id);
  std::string get_account_by_id(uint64_t account_id) override;
  uint64_t get_balance_by_account_id_asset_id(uint64_t account_id, uint64_t asset_id);
  void drop_tables();
 private:
  pqxx::connection connection_;
};
}
}

#endif //AMETSUCHI_WSV_POSTGRES_H
