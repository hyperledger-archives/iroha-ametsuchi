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

#include <gtest/gtest.h>
#include <nudb/nudb.hpp>

TEST(NuDB_test, sample_test) {
  using namespace nudb;
  std::size_t constexpr N = 1000;
  using key_type = std::uint32_t;
  error_code ec;
  auto const dat_path = "db.dat";
  auto const key_path = "db.key";
  auto const log_path = "db.log";
  create<xxhasher>(
    dat_path, key_path, log_path,
    1,
    make_salt(),
    sizeof(key_type),
    block_size("."),
    0.5f,
    ec);
  store db;
  db.open(dat_path, key_path, log_path, ec);
  char data = 0;
  // Insert
  for(key_type i = 0; i < N; ++i)
    db.insert(&i, &data, sizeof(data), ec);
  // Fetch
  for(key_type i = 0; i < N; ++i)
    db.fetch(&i,
             [&](void const* buffer, std::size_t size)
             {
                 ASSERT_EQ(*(char*)buffer, 0);
                 ASSERT_EQ(size, 1);
             }, ec);
  db.close(ec);
  erase_file(dat_path);
  erase_file(key_path);
  erase_file(log_path);
}
