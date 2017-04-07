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
class MDB_val;  // forward declaration

class Ametsuchi {
 public:
  explicit Ametsuchi(const std::string &db_folder);

  void append(const ByteArray &tx);
  void append(const std::vector<ByteArray> &batch);
  void commit();
  void rollback();

  // std::vector<const MDB_val> getAddTxByCreator(const std::string &pubKey);

  /**
 * Returns all assets, which belong to user with \p pubKey.
 * @param pubKey - account's public key
 * @param ln - ledger name
 * @param dn - domain name
 * @param an - asset (currency) name
 * @return 0 or * blobs, which are mmaped into memory.
 */
  std::vector<MDB_val> accountGetAssets(const flatbuffers::String *pubKey);

  /**
   * Returns specific asset, which belong to user with \p pubKey.
   * @param pubKey - account's public key
   * @param ln - ledger name
   * @param dn - domain name
   * @param an - asset (currency) name
   * @return
   */
  MDB_val accountGetAsset(const flatbuffers::String *pubKey,
                          const flatbuffers::String *ln,
                          const flatbuffers::String *dn,
                          const flatbuffers::String *an);

 private:
  std::string path_;
  MDB_env *env;
  MDB_stat mst;
  MDB_txn *append_tx_;  // pointer to db transaction
  std::unordered_map<std::string, std::pair<MDB_dbi, MDB_cursor *>> trees_;
  size_t tx_store_total;

  void init();
  void init_btree(const std::string &name, uint32_t flags,
                  MDB_cmp_func *dupsort = NULL);
  size_t tx_store_size();
  void init_append_tx();
  void abort_append_tx();
  void read_created_assets();


  // handlers for transactions
  void account_add(const iroha::AccountAdd *command);
  bool account_remove(const iroha::AccountRemove *command);
  void peer_add(const iroha::PeerAdd *command);
  void peer_remove(const iroha::PeerRemove *command);
  void asset_create(const iroha::AssetCreate *command);
  bool asset_add(const iroha::AssetAdd *command);
  bool asset_remove(const iroha::AssetRemove *command);
  bool asset_transfer(const iroha::AssetTransfer *command);


  // [ledger+domain+asset] => ComplexAsset/Currency flatbuffer (without amount)
  std::unordered_map<std::string, std::vector<uint8_t>> created_assets_;
  std::vector<std::pair<MDB_val, MDB_val>> read_all_records(
      const std::string &tree_name);
};

}  // namespace ametsuchi

#endif  // AMETSUCHI_DB_H
