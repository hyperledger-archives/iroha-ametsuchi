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
#include <block_store_flat.h>
#include <block_index_redis.h>
#include <hash.h>
#include <block_index_mediator.h>

class BlockIndexMediatorTest : public ::testing::Test {
 protected:
  virtual void TearDown() {
    cpp_redis::redis_client client;
    client.connect(host_, port_);
    client.flushall();
    client.disconnect();
    system(("rm -rf " + block_store_path).c_str());
  }

  std::string host_ = std::getenv("REDISHOST");
  size_t port_ = 6379;

  std::string block_store_path = "/tmp/dump";
};

TEST_F(BlockIndexMediatorTest, valid){
  block_store::BlockStoreFlat bl_store(block_store_path);
  block_index::BlockIndexRedis bl_index(host_, port_);

  std::vector<uint8_t > blob1({0x1, 0x2, 0x2});
  std::vector<uint8_t > blob2({0x3, 0x4});

  std::string hash1(32, '\0');
  utils::sha3_256((unsigned char *)&hash1.at(0), blob1.data(), blob1.size());

  std::string hash2(32, '\0');
  utils::sha3_256((unsigned char *)&hash2.at(0), blob2.data(), blob2.size());

  uint32_t id1 = 1u;
  bl_store.add(id1, blob1);
  bl_index.add_blockhash_blockid(hash1, id1);

  uint32_t id2 = 2u;
  bl_store.add(id2, blob2);
  bl_index.add_blockhash_blockid(hash2, id2);

  ASSERT_EQ(bl_index.get_last_blockid(), bl_store.last_id());

  crash_handler::BlockIndexMediator bim(bl_index, bl_store);
  ASSERT_TRUE(bim.validate());
  ASSERT_EQ(bl_index.get_blockid_by_blockhash(hash1), id1);
  ASSERT_EQ(bl_index.get_blockid_by_blockhash(hash2), id2);
}

TEST_F(BlockIndexMediatorTest, invalid_to_valid){
  block_store::BlockStoreFlat bl_store(block_store_path);
  block_index::BlockIndexRedis bl_index(host_, port_);

  std::vector<uint8_t > blob1({0x1, 0x2, 0x2});
  std::vector<uint8_t > blob2({0x3, 0x4});

  std::string hash1(32, '\0');
  utils::sha3_256((unsigned char *)&hash1.at(0), blob1.data(), blob1.size());

  std::string hash2(32, '\0');
  utils::sha3_256((unsigned char *)&hash2.at(0), blob2.data(), blob2.size());

  uint32_t id1 = 1u;
  bl_store.add(id1, blob1);
  bl_index.add_blockhash_blockid(hash1, id1);

  // The last blob is added only to bl_store, thus, it is needed to make them consistent
  uint32_t id2 = 2u;
  bl_store.add(id2, blob2);

  ASSERT_NE(bl_index.get_last_blockid(), bl_store.last_id());

  ASSERT_EQ(bl_index.get_blockid_by_blockhash(hash1), id1);

  crash_handler::BlockIndexMediator bim(bl_index, bl_store);
  ASSERT_TRUE(bim.validate());
  ASSERT_EQ(bl_index.get_last_blockid(), bl_store.last_id());

  ASSERT_EQ(bl_index.get_blockid_by_blockhash(hash1), id1);
  ASSERT_EQ(bl_index.get_blockid_by_blockhash(hash2), id2);
}

TEST_F(BlockIndexMediatorTest, b_index_is_lost){
  block_store::BlockStoreFlat bl_store(block_store_path);

  std::vector<uint8_t > blob1({0x1, 0x2, 0x2});
  std::vector<uint8_t > blob2({0x3, 0x4});

  std::string hash1(32, '\0');
  utils::sha3_256((unsigned char *)&hash1.at(0), blob1.data(), blob1.size());

  std::string hash2(32, '\0');
  utils::sha3_256((unsigned char *)&hash2.at(0), blob2.data(), blob2.size());

  uint32_t id1 = 1u;
  bl_store.add(id1, blob1);

  // The last blob is added only to bl_store, thus, it is needed to make them consistent
  uint32_t id2 = 2u;
  bl_store.add(id2, blob2);

  block_index::BlockIndexRedis bl_index(host_, port_);
  crash_handler::BlockIndexMediator bim(bl_index, bl_store);
  ASSERT_TRUE(bim.validate());
  ASSERT_EQ(bl_index.get_last_blockid(), bl_store.last_id());

  ASSERT_EQ(bl_index.get_blockid_by_blockhash(hash1), id1);
  ASSERT_EQ(bl_index.get_blockid_by_blockhash(hash2), id2);
}