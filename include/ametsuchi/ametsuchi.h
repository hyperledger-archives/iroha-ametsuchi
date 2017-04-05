/**
 * Copyright Soramitsu Co., Ltd. 2016 All Rights Reserved.
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

#ifndef AMETSUCHI_DB_H
#define AMETSUCHI_DB_H

#include <ametsuchi/generated/commands_generated.h>
#include <lmdb.h>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>
#include "currency.h"

namespace ametsuchi {

using ByteArray = std::vector<uint8_t>;

class Ametsuchi {
 public:
  explicit Ametsuchi(const std::string &db_folder);

  void append(const ByteArray &tx);
  void append(const std::vector<ByteArray> &batch);
  void commit();
  void rollback();

  std::vector<ByteArray> getAddTxByCreator(const std::string &pubKey);

 private:
  std::string path_;

  void init();
  void init_btree(const std::string &name, uint32_t flags);
  size_t tx_store_size();
  void init_append_tx();
  void abort_append_tx();

  MDB_env *env;
  MDB_stat mst;

  MDB_txn *append_tx_;  // pointer to db transaction

  std::unordered_map<std::string, std::pair<MDB_dbi, MDB_cursor *>> trees_;

  size_t tx_store_total;

  bool account_add(const iroha::AccountAdd *command);
  bool peer_add(const iroha::PeerAdd *command);
  bool asset_create(const iroha::AssetCreate *command);
  void asset_add(const iroha::AssetAdd *command);

  std::unordered_set<std::string> created_assets_;
};

}  // namespace ametsuchi

#endif  // AMETSUCHI_DB_H
