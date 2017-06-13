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

#include <cppfs/FileHandle.h>
#include <cppfs/fs.h>
#include <grpc++/grpc++.h>
#include <gtest/gtest.h>
#include <hash.h>
#include <cpp_redis/cpp_redis>
#include <pqxx/pqxx>
#include "storage_service.h"

namespace service {

class ServiceTest : public ::testing::Test {
 protected:
  virtual void TearDown() {
    cppfs::fs::open("/tmp/block_store").removeDirectoryRec();

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
  std::string server_address("0.0.0.0:50051");
  StorageServiceImpl service;

  grpc::ServerBuilder builder;
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  builder.RegisterService(&service);
  auto server = builder.BuildAndStart();

  auto stub = iroha::Storage::NewStub(grpc::CreateChannel(
      "localhost:50051", grpc::InsecureChannelCredentials()));

  iroha::Block sent_block;
  {
    sent_block.mutable_meta()->set_height(1);
    sent_block.mutable_meta()->set_size(1);
    auto transaction = sent_block.mutable_body()->add_transaction();
    auto action = transaction->mutable_body()->add_action();
    std::string public_key = "00000000000000000000000000000000";
    auto signature = action->mutable_add_account()->add_signature();
    signature->set_public_key(public_key);
    grpc::ClientContext context;
    iroha::BlockMessage request;
    request.set_allocated_block(&sent_block);
    google::protobuf::Empty response;
    auto status = stub->AddBlock(&context, request, &response);
    request.release_block();
    ASSERT_TRUE(status.ok());
  }
  // Block hash
  std::string block_hash(32, '\0');
  std::vector<uint8_t> buffer(sent_block.ByteSizeLong());
  sent_block.meta().SerializeToArray(buffer.data(),
                                     sent_block.meta().ByteSize());
  sent_block.body().SerializeToArray(
      buffer.data() + sent_block.meta().ByteSizeLong(),
      sent_block.body().ByteSize());
  utils::sha3_256(
      (unsigned char *)&block_hash[0], buffer.data(),
      sent_block.meta().ByteSizeLong() + sent_block.body().ByteSizeLong());

  {
    grpc::ClientContext context;
    iroha::BlockIdMessage request;
    request.set_block_hash(block_hash);
    iroha::BlockMessage response;
    auto status = stub->GetBlock(&context, request, &response);
    ASSERT_TRUE(status.ok());
    ASSERT_TRUE(response.block().has_meta());
    ASSERT_TRUE(response.block().has_body());
    ASSERT_EQ(response.block().meta().height(), sent_block.meta().height());
    ASSERT_EQ(response.block().meta().size(), sent_block.meta().size());
    ASSERT_EQ(response.block().body().transaction_size(), 1);
    ASSERT_EQ(response.block().body().transaction(0).body().action_size(), 1);
    ASSERT_TRUE(response.block()
                    .body()
                    .transaction(0)
                    .body()
                    .action(0)
                    .has_add_account());
    ASSERT_EQ(response.block()
                  .body()
                  .transaction(0)
                  .body()
                  .action(0)
                  .add_account()
                  .signature_size(),
              1);
    ASSERT_EQ(response.block()
                  .body()
                  .transaction(0)
                  .body()
                  .action(0)
                  .add_account()
                  .signature(0)
                  .public_key(),
              "00000000000000000000000000000000");
  }
}

TEST_F(ServiceTest, postgres_test) {
  backend_ = "Postgres";
  test_backend(backend_);
}

// TEST_F(ServiceTest, redis_test) {
//  backend_ = "Redis";
//  test_backend(backend_);
//}
}
