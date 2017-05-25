#include <gtest/gtest.h>
#include <ametsuchi/block_index/block_index_redis.h>

class BlockIndex_Test : public ::testing::Test {
 protected:
  virtual void TearDown() {
    cpp_redis::redis_client client;
    client.connect(host_, port_);
    client.flushall();
    client.disconnect();
  }

  std::string host_ = "10.90.130.160";
  size_t port_ = 6379;
};


TEST_F(BlockIndex_Test, REDIS_ADD_GET_TEST){
  ametsuchi::block_index::BlockIndexRedis block_index(host_, port_);

  block_index.add_blockhash_blockid("one", 1);
  size_t one = block_index.get_blockid_by_blockhash("one");
  ASSERT_EQ(one, 1);
}