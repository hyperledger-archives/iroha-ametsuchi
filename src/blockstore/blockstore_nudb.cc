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

#include <blockstore/blockstore_nudb.h>

namespace blockstore {

void TxStoreNuDB::append(const std::vector<uint8_t> &tx, int res) {
  nudb::error_code ec;
  db_.insert(&size_, tx.data(), tx.size(), ec);
  ++size_;
}

std::vector<uint8_t> TxStoreNuDB::get(size_t index, int res) {
  nudb::error_code ec;
  std::vector<uint8_t> tx;
  db_.fetch(&index, [&](void const* buffer, std::size_t size){
    tx = {(uint8_t*)buffer, (uint8_t*)buffer + size};
  }, ec);
  return tx;
}

TxStoreNuDB::TxStoreNuDB() {
  const auto dat_file = "nudb.dat", key_file = "nudb.key", log_file = "nudb.log";
  const auto load_factor = .5f;
  nudb::error_code ec;
  nudb::create<nudb::xxhasher>(dat_file, key_file, log_file, 1, nudb::make_salt(),
               sizeof(size_t), nudb::block_size("."), load_factor, ec);
  db_.open(dat_file, key_file, log_file, ec);
  nudb::native_file df;
  df.open(nudb::file_mode::scan, dat_file, ec);
  size_ = df.size(ec);
  df.close();
}
TxStoreNuDB::~TxStoreNuDB() {
  nudb::error_code ec;
  db_.close(ec);
}
}
