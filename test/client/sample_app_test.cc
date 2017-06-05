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

#include <ametsuchi/ametsuchi.h>
#include <grpc++/grpc++.h>
#include <gtest/gtest.h>
#include <atomic>
#include <thread>

class FakeQueryServiceImpl final : public iroha::Query::Service {
 public:
  grpc::Status GetAccount(::grpc::ServerContext *context,
                          const ::iroha::AccountRequest *request,
                          ::iroha::AccountResponse *response) override {
    response->set_name("Ivan");
    return grpc::Status::OK;
  }
  grpc::Status GetBalance(::grpc::ServerContext *context,
                          const ::iroha::BalanceRequest *request,
                          ::iroha::BalanceResponse *response) override {
    response->set_amount(100);
    return grpc::Status::OK;
  }
};

class FakeCommandServiceImpl final : public iroha::Command::Service {
 public:
  grpc::Status Append(::grpc::ServerContext *context,
                      const ::iroha::AppendRequest *request,
                      ::iroha::AppendResponse *response) override {
    response->set_id(1);
    return grpc::Status::OK;
  }
};

TEST(sample_app_test, sample_test) {
  std::string server_address("0.0.0.0:50051");
  FakeQueryServiceImpl queryService;
  FakeCommandServiceImpl commandService;

  grpc::ServerBuilder builder;
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  builder.RegisterService(&queryService);
  builder.RegisterService(&commandService);
  auto server = builder.BuildAndStart();

  ametsuchi::Client client;
  ASSERT_EQ(client.get_account_by_id(0), "Ivan");
  ASSERT_EQ(client.get_balance_by_account_id_asset_id(0, 0), 100);
  ASSERT_EQ(client.append(new iroha::Block), 1);
  server->Shutdown();
}
