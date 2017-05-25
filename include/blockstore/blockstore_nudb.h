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

#ifndef AMETSUCHI_TX_STORE_NUDB_H
#define AMETSUCHI_TX_STORE_NUDB_H

#include <nudb/nudb.hpp>
#include "blockstore.h"

namespace blockstore {
class TxStoreNuDB : public TxStore {
 public:
  TxStoreNuDB();
  void append(const std::vector<uint8_t> &tx, int res) override;
  std::vector<uint8_t> get(size_t index, int res) override;
  ~TxStoreNuDB();
 private:
  nudb::store db_;
  size_t size_;
};
}

#endif  // AMETSUCHI_TX_STORE_NUDB_H
