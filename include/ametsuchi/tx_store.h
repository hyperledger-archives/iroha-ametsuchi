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

#ifndef AMETSUCHI_TX_STORE_H
#define AMETSUCHI_TX_STORE_H

#include <ametsuchi/common.h>
#include <ametsuchi/merkle_tree/merkle_tree.h>
#include <commands_generated.h>
#include <flatbuffers/flatbuffers.h>
#include <lmdb.h>
#include <unordered_map>

namespace std {
    template <class T>
    struct hash {
        static_assert(is_enum<T>::value, "This hash only works for enumeration types");
        size_t operator()(T x) const noexcept {
          using type = typename underlying_type<T>::type;
          return hash<type>{}(static_cast<type>(x));
        }
    };
}

namespace ametsuchi {

class TxStore {
 public:
  TxStore(size_t merkle_leaves);
  ~TxStore();

  void commit();

  void init_merkle_tree();

  merkle::hash_t merkle_root();

  merkle::hash_t append(const std::vector<uint8_t> *blob);
  void init(MDB_txn *append_tx);

  /**
   * Close every cursor used in tx_store
   */
  void close_cursors();

  /**
 * Close every dbi used in tx_store
 */
  void close_dbi(MDB_env *env);

  /*
   * Get number of trees used in tx_store
   */
  uint32_t get_trees_total();

  // TxStore queries:

  std::vector<AM_val> getAssetTransferBySender(
      const flatbuffers::String *senderKey, bool uncommitted = true,
      MDB_env *env = nullptr);

  std::vector<AM_val> getAssetTransferByReceiver(
      const flatbuffers::String *receiverKey, bool uncommitted = true,
      MDB_env *env = nullptr);

  std::vector<AM_val> getCommandByKey(const flatbuffers::String *pubKey,
                                      iroha::Command command,
                                      bool uncommitted = true,
                                      MDB_env *env = nullptr);

 private:
  size_t tx_store_total;
  std::unordered_map<std::string, std::pair<MDB_dbi, MDB_cursor *>> trees_;
  std::unordered_map<iroha::Command, std::string> command_tree_name_;

  merkle::MerkleTree merkleTree_;

  MDB_txn *append_tx_;
  void set_tx_total();
  uint32_t TX_STORE_TREES_TOTAL;
  void put_tx_into_tree_by_key(MDB_cursor *cursor,
                               const flatbuffers::String *acc_pub_key,
                               size_t &tx_store_total);

  void create_new_tree(MDB_txn *append_tx, const std::string &name,
                       uint32_t flags, MDB_cmp_func *dupsort = nullptr);


  std::vector<AM_val> getTxByKey(const std::string &tree_name,
                                 const flatbuffers::String *pubKey,
                                 bool uncommitted = true,
                                 MDB_env *env = nullptr);
};
}


#endif  // AMETSUCHI_TX_STORE_H
