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

#include <flatbuffers/flatbuffers.h>
#include <lmdb.h>
#include <unordered_map>
#include "common.h"

namespace ametsuchi {

class TxStore {
 public:
  TxStore();
  ~TxStore();

  void append(const flatbuffers::Vector<uint8_t> *blob);
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


  std::vector<AM_val> getAssetTxByCreator(const flatbuffers::String *pubKey,
                                          bool uncommitted = true,
                                          MDB_env *env = nullptr);

  std::vector<AM_val> getAccountTxByCreator(const flatbuffers::String *pubKey,
                                            bool uncommitted = true,
                                            MDB_env *env = nullptr);

  std::vector<AM_val> getPeerTxByCreator(const flatbuffers::String *pubKey,
                                         bool uncommitted = true,
                                         MDB_env *env = nullptr);

  std::vector<AM_val> getAssetTxBySender(const flatbuffers::String *senderKey,
                                         bool uncommitted = true,
                                         MDB_env *env = nullptr);

  std::vector<AM_val> getAssetTxByReceiver(const flatbuffers::String *receiverKey,
                                           bool uncommitted = true,
                                           MDB_env *env = nullptr);

 private:
  size_t tx_store_total;
  std::unordered_map<std::string, std::pair<MDB_dbi, MDB_cursor *>> trees_;
  MDB_txn *append_tx_;
  void set_tx_total();
  uint32_t TX_STORE_TREES_TOTAL;
  void put_creator_into_tree(MDB_cursor *cursor,
                             const flatbuffers::String *acc_pub_key,
                             size_t &tx_store_total);


  std::vector<AM_val> getTxByCreator(const std::string &tree_name,
                                     const flatbuffers::String *pubKey,
                                     bool uncommitted = true,
                                     MDB_env *env = nullptr);
};
}


#endif  // AMETSUCHI_TX_STORE_H
