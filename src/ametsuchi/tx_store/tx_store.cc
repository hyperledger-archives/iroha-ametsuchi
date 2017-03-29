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


#include <ametsuchi/tx_store/array.h>
#include <ametsuchi/tx_store/tx_store.h>

namespace ametsuchi {
namespace tx_store {

// TODO(warchant): we need better management for database files
static db_path = Eng::get().get_database_directory();
static Array STORE = Array(db_path + "/ledger");


void append(const std::vector<TX> &batch) {}

void append(const TX &tx) {}

std::vector<TX> getAddTxByCreator(const std::string &publicKey) {}

std::vector<TX> getTransferTxBySender(const std::string &publicKey) {}

std::vector<TX> getTransferTxByReceiver(const std::string &publicKey) {}

bool commit() {}

bool rollback() {}

}  // namespace tx_store
}  // namespace ametsuchi
