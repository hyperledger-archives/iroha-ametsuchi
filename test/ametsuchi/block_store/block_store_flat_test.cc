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

#include <ametsuchi/tx_index/tx_index_redis.h>
#include <gtest/gtest.h>
#include <ametsuchi/block_store/block_store_flat.h>

class BlStore_Test : public ::testing::Test {
 protected:
  virtual void TearDown() {

  }

};

TEST(BlStore_Test, Read_Write_Test) {

  std::vector<uint8_t > block(100000, 5);
  ametsuchi::block_store::BlockStoreFlat bl_store("dump");
  auto id = bl_store.append(block);
  auto id2 = bl_store.append(block);
  auto res = bl_store.get(id);
  ASSERT_FALSE(res.empty());
  ASSERT_EQ(res, block);
}
