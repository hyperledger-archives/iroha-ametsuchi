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

#include <ametsuchi/block_store/block_store_nudb.h>
#include <iostream>

namespace ametsuchi {
namespace block_store {

void BlockStoreNuDB::append(size_t index, const std::string &block) {
  nudb::error_code ec;
  db_.insert(&size_, block.data(), block.size(), ec);
  ++size_;
}

std::string BlockStoreNuDB::get(size_t index) {
  nudb::error_code ec;
  std::string block;
  db_.fetch(&index,
            [&](void const *buffer, std::size_t size) {
              block = {(uint8_t *) buffer, (uint8_t *) buffer + size};
            },
            ec);
  return block;
}

BlockStoreNuDB::BlockStoreNuDB() {
  const auto dat_file = "nudb.dat", key_file = "nudb.key",
    log_file = "nudb.log";
  const auto load_factor = .5f;
  nudb::error_code ec;
  nudb::create<nudb::xxhasher>(dat_file, key_file, log_file, 1,
                               nudb::make_salt(), sizeof(size_t),
                               nudb::block_size("."), load_factor, ec);
  if (ec == nudb::errc::file_exists) {
    ec = {};
  }
  db_.open(dat_file, key_file, log_file, ec);
  nudb::visit(dat_file, [&](void const* key, std::size_t keySize,
                            void const* data, std::size_t dataSize,
                            nudb::error_code& code)  {
    ++size_;
  }, nudb::no_progress{}, ec);
}

BlockStoreNuDB::~BlockStoreNuDB() {
  nudb::error_code ec;
  db_.close(ec);
}

}
}
