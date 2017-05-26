#include <gtest/gtest.h>
#include <ametsuchi/tx_index/tx_index_redis.h>

class TxIndex_Test : public ::testing::Test {
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

TEST_F(TxIndex_Test, TxRedisTest){
  ametsuchi::tx_index::TxIndexRedis tx_index(host_, port_);

  tx_index.add_txhash_blockhash_txid("tx_one", 1, 10);

  auto one = tx_index.get_blockhash_by_txhash("tx_one");
  auto ten = tx_index.get_txid_by_txhash("tx_one");

  ASSERT_EQ(one, 1);
  ASSERT_EQ(ten, 10);
}
