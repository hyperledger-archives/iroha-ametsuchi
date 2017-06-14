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

#include <block_index_redis.h>
#include <gtest/gtest.h>

class BlockIndex_Test : public ::testing::Test {
 protected:
  virtual void TearDown() {
    cpp_redis::redis_client client;
    client.connect(host_, port_);
    client.flushall();
    client.sync_commit();
    client.disconnect();
  }

  std::string host_ = std::getenv("REDISHOST");
  size_t port_ = 6379;
};

TEST_F(BlockIndex_Test, REDIS_ADD_GET_TEST) {
  block_index::BlockIndexRedis block_index;

  block_index.add_blockhash_blockid("one", 1);
  uint32_t one = block_index.get_blockid_by_blockhash("one");
  ASSERT_EQ(one, 1);
}
