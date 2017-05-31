#include <ametsuchi/ametsuchi.h>
#include <gtest/gtest.h>
#include <cpp_redis/redis_client.hpp>
#include <nudb/nudb.hpp>

class Ametsuchi_Test : public ::testing::Test {
 protected:
  virtual void TearDown() {
    cpp_redis::redis_client client;
    client.connect(host_, port_);
    client.flushall();
    client.disconnect();

    nudb::erase_file(dat_file);
    nudb::erase_file(key_file);
    nudb::erase_file(log_file);
  }

  std::string host_ = ametsuchi::wsv_host;
  size_t port_ = ametsuchi::wsv_port;
  const std::string dat_file = "nudb.dat", key_file = "nudb.key",
             log_file = "nudb.log";
};


TEST_F(Ametsuchi_Test, Ametsuchi_Test) {
  ametsuchi::Ametsuchi succi;
  // BlockHash#Tx#TX
  std::string block =
      "2345234#12_account 1 bulat_account 2 andrei_balance 1 10_balance 2 "
      "10#22_account 3 kamil_balance 3 100";
  std::vector<uint8_t> block_vector = {block.data(), block.data() + block.size()};
  succi.append(block);
  ASSERT_EQ(succi.get_balance_by_accountid(1), 10);
  ASSERT_EQ(succi.get_balance_by_accountid(2), 10);
  ASSERT_EQ(succi.get_balance_by_accountid(3), 100);

  ASSERT_EQ(succi.get_transaction_by_hash("12"), "12_account 1 bulat_account 2 andrei_balance 1 10_balance 2 10");
  ASSERT_EQ(succi.get_transaction_by_hash("22"), "22_account 3 kamil_balance 3 100");

  ASSERT_EQ(succi.get_block_by_hash("2345234"), block_vector);

}
