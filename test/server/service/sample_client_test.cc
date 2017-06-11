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

#include <grpc++/grpc++.h>
#include <gtest/gtest.h>
#include <cpp_redis/cpp_redis>
#include <pqxx/pqxx>
#include "storage_service.h"

namespace service {

class ServiceTest : public ::testing::Test {
 protected:
  virtual void TearDown() {
    std::remove("/tmp/block_store/0000000000000001");
    std::remove("/tmp/block_store/0000000000000002");
    std::remove("/tmp/block_store/0000000000000003");
    std::remove("/tmp/block_store");

    if (backend_ == "Postgres") {
      const auto drop =
          "DROP TABLE IF EXISTS domain_has_account;\n"
          "DROP TABLE IF EXISTS account_has_asset;\n"
          "DROP TABLE IF EXISTS asset;\n"
          "DROP TABLE IF EXISTS domain;\n"
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
  std::string server_address("0.0.0.0:50051");
  StorageServiceImpl service;

  grpc::ServerBuilder builder;
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  builder.RegisterService(&service);
  auto server = builder.BuildAndStart();

  auto stub = iroha::Storage::NewStub(grpc::CreateChannel(
      "localhost:50051", grpc::InsecureChannelCredentials()));

  // TODO
}

TEST_F(ServiceTest, postgres_test) {
  backend_ = "Postgres";
  test_backend(backend_);
}

//TEST_F(ServiceTest, redis_test) {
//  backend_ = "Redis";
//  test_backend(backend_);
//}
}
