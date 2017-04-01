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

#include <ametsuchi/generated/transaction_generated.h>
#include <ametsuchi/tx_store/array.h>
#include <ametsuchi/tx_store/iterator.h>
#include <ametsuchi/tx_store/tx_store.h>

namespace ametsuchi {
namespace tx_store {


// TODO(warchant): we need better management for database files
static Array STORE(Env::get().get_database_directory() + "/ledger");

void append(const std::vector<ByteArray> &batch) { STORE.append_batch(batch); }

void append(const ByteArray &tx) { STORE.append(tx); }

void commit() { STORE.commit(); }

void rollback() { STORE.rollback(); }

// TODO: create SQLite index
std::vector<ByteArray> getAddTxByCreator(const std::string &publicKey) {
  std::vector<ByteArray> ret;
  for (auto &&it : STORE) {
    ret.push_back(it);
  }
  return ret;
}

std::vector<ByteArray> getTransferTxBySender(const std::string &publicKey) {
  throw "not implemented;";
}

std::vector<ByteArray> getTransferTxByReceiver(const std::string &publicKey) {
  throw "not implemented;";
}

}  // namespace tx_store
}  // namespace ametsuchi
