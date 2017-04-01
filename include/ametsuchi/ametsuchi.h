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

#include <ametsuchi/env.h>
#include <ametsuchi/globals.h>
#include <ametsuchi/tx_store/array.h>
#include <lmdb.h>
#include <string>
#include <vector>

namespace ametsuchi {

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

  std::unique_ptr<tx_store::Array> tx_;

  void open_append_tx();
  void abort_append_tx();

  void create_tx_index();
  void append_index(const ByteArray &blob);

  MDB_env *env;
  MDB_stat mst;

  MDB_txn *append_tx;    // pointer to db transaction
  MDB_dbi dbi_index1;    // "Add" transactions by creator key
  MDB_dbi dbi_index2;    // "Transfer" transactions by sender key
  MDB_dbi dbi_index3;    // "Transfer" transactions by receiver key
  MDB_cursor *cursor_1;  // append cursor for index1
  MDB_cursor *cursor_2;  // append cursor for index2
  MDB_cursor *cursor_3;  // append cursor for index3
};
}

#endif  // AMETSUCHI_DB_H
