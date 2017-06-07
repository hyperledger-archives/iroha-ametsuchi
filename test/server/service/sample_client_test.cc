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
#include "storage_service.h"
#include <grpc++/grpc++.h>
#include <pqxx/pqxx>
#include <cpp_redis/cpp_redis>

namespace service {

class ServiceTest : public ::testing::Test {
 protected:
  virtual void TearDown() {
    std::remove("/tmp/block_store/0000000000000001");
    std::remove("/tmp/block_store/0000000000000002");
    std::remove("/tmp/block_store/0000000000000003");
    std::remove("/tmp/block_store");
  }
};

void test_backend(const std::string &backend) {
  std::string server_address("0.0.0.0:50051");
  StorageServiceImpl service(backend);

  grpc::ServerBuilder builder;
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  builder.RegisterService(&service);
  auto server = builder.BuildAndStart();

  auto stub = iroha::Storage::NewStub(grpc::CreateChannel(
    "localhost:50051", grpc::InsecureChannelCredentials()));

  {
    iroha::AppendRequest request;
    auto block = request.mutable_block();
    auto txs = block->add_txs();
    auto actions = txs->add_actions();
    auto add_account = actions->mutable_add_account();
    add_account->set_account_id(0);
    add_account->set_name("Ivan");
    iroha::AppendResponse response;
    grpc::ClientContext context;
    auto status = stub->Append(&context, request, &response);
    ASSERT_TRUE(status.ok());
    ASSERT_EQ(response.id(), 1);
  }
  {
    iroha::AccountRequest request;
    request.set_account_id(0);
    iroha::AccountResponse reply;
    grpc::ClientContext context;
    auto status = stub->GetAccount(&context, request, &reply);
    ASSERT_TRUE(status.ok());
    ASSERT_EQ(reply.name(), "Ivan");
  }
  {
    iroha::AppendRequest request;
    auto block = request.mutable_block();
    auto txs = block->add_txs();
    auto actions = txs->add_actions();
    auto add_domain = actions->mutable_add_domain();
    add_domain->set_domain_id(0);
    add_domain->set_name("RU");
    add_domain->set_root_account_id(0);
    actions = txs->add_actions();
    auto add_asset = actions->mutable_add_asset();
    add_asset->set_asset_id(0);
    add_asset->set_domain_id(0);
    add_asset->set_name("USD");
    iroha::AppendResponse response;
    grpc::ClientContext context;
    auto status = stub->Append(&context, request, &response);
    ASSERT_TRUE(status.ok());
    ASSERT_EQ(response.id(), 2);
  }
  {
    iroha::AppendRequest request;
    auto block = request.mutable_block();
    auto txs = block->add_txs();
    auto actions = txs->add_actions();
    auto add_balance = actions->mutable_add_balance();
    add_balance->set_account_id(0);
    add_balance->set_asset_id(0);
    add_balance->set_amount(100);
    iroha::AppendResponse response;
    grpc::ClientContext context;
    auto status = stub->Append(&context, request, &response);
    ASSERT_TRUE(status.ok());
    ASSERT_EQ(response.id(), 3);
  }
  {
    iroha::BalanceRequest request;
    request.set_account_id(0);
    request.set_asset_id(0);
    iroha::BalanceResponse reply;
    grpc::ClientContext context;
    auto status = stub->GetBalance(&context, request, &reply);
    ASSERT_TRUE(status.ok());
    ASSERT_EQ(reply.amount(), 100);
  }
}

TEST_F(ServiceTest, postgres_test) {
  test_backend("Postgres");

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
}

TEST_F(ServiceTest, redis_test) {
  test_backend("Redis");
  cpp_redis::redis_client client;
  client.connect();
  client.flushall();
  client.sync_commit();
  client.disconnect();
}

}
