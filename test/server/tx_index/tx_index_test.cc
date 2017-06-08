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
#include <tx_index_redis.h>

class TxIndex_Test : public ::testing::Test {
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

TEST_F(TxIndex_Test, TxRedisTest) {
  tx_index::TxIndexRedis tx_index(host_, port_);

  tx_index.add_txhash_blockhash_txid("tx_one", std::to_string(1),
                                     std::to_string(10));

  auto one = tx_index.get_blockhash_by_txhash("tx_one");
  auto ten = tx_index.get_txid_by_txhash("tx_one");

  ASSERT_EQ(one, std::to_string(1));
  ASSERT_EQ(ten, std::to_string(10));
}
