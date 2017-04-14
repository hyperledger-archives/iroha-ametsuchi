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


 private:
  size_t tx_store_total;
  std::unordered_map<std::string, std::pair<MDB_dbi, MDB_cursor *>> trees_;
  MDB_txn *append_tx_;
  void set_tx_total();
};
}


#endif  // AMETSUCHI_TX_STORE_H
