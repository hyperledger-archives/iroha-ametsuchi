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
#include <manager.h>
#include <wsv.h>
#include <cpp_redis/cpp_redis>
#include <pqxx/pqxx>

class WSVTest : public ::testing::Test {
 protected:
  virtual void TearDown() {
    if (backend_ == "Postgres") {
      const auto drop =
          "DROP TABLE IF EXISTS domain_has_account;\n"
            "DROP TABLE IF EXISTS account_has_asset;\n"
            "DROP TABLE IF EXISTS account_has_wallet;\n"
            "DROP TABLE IF EXISTS wallet;\n"
            "DROP TABLE IF EXISTS exchange;\n"
            "DROP TABLE IF EXISTS asset;\n"
            "DROP TABLE IF EXISTS domain;\n"
            "DROP TABLE IF EXISTS peer;\n"
            "DROP TABLE IF EXISTS signatory;\n"
            "DROP TABLE IF EXISTS account;";

      pqxx::connection connection;
      pqxx::work txn(connection);
      txn.exec(drop);
      txn.commit();
      connection.disconnect();
    } else if (backend_ == "Redis") {
      cpp_redis::redis_client client;
      client.connect(redis_host_, redis_port_);
      client.flushall();
      client.sync_commit();
      client.disconnect();
    }
  }
  std::string backend_;
  std::string redis_host_ =
      std::getenv("REDISHOST") ? std::getenv("REDISHOST") : "127.0.0.1";
  size_t redis_port_ =
      std::getenv("REDISPORT") ? std::stoull(std::getenv("REDISPORT")) : 6379;
};

void test_backend(const std::string &backend) {
  auto wsv_ = wsv::Manager::instance().make_WSV(backend);
  std::string public_key("00000000000000000000000000000000");
  ASSERT_TRUE(wsv_->add_account(public_key, 1, 1));
  ASSERT_TRUE(wsv_->add_signatory(public_key, public_key));
  std::string address("127.0.0.1");
  ASSERT_TRUE(wsv_->add_peer(public_key, address, 1));
  auto result = wsv_->get_peers();
  ASSERT_EQ(result.size(), 1);
  ASSERT_EQ(result.at(0), address);
}

TEST_F(WSVTest, postgres_test) {
  backend_ = "Postgres";
  test_backend(backend_);
}

//TEST_F(WSVTest, redis_test) {
//  backend_ = "Redis";
//  test_backend(backend_);
//}
