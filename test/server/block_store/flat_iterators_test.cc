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

#include <block_store_flat.h>
#include <gtest/gtest.h>
#include <flat_iterator.h>

class BlStoreIterators_Test : public ::testing::Test {
 protected:
  virtual void TearDown() {}
};

TEST(BlStoreIterators_Test, Forward_Test) {
  uint n = 5u;
  block_store::BlockStoreFlat bl_store("dump_test");
  std::vector<std::vector<uint8_t>> test_set(n);

  auto offset = bl_store.last_id();

  for (uint i = 0u; i < n; ++i) {
    test_set[i] = std::vector<uint8_t>(i+1, 5);
    auto id = bl_store.append(test_set[i]);
    ASSERT_EQ(id, i+1+offset);
  }

  int i = 0;
  auto end = bl_store.begin()+n;
  auto begin = bl_store.begin();
  for (auto it = begin; it < end; ++it) {

    ASSERT_EQ(test_set[i], *it);
    i++;
  }


}
