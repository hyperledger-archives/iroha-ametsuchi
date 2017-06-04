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
#include <thread>
#include <manager.h>
#include "query_service.h"

namespace service {

TEST(sample_client_test, sample_test) {
  for (auto backend: {"Postgres", "Redis"}) {
    std::unique_ptr<wsv::WSV>
      wsv_ = wsv::Manager::instance().make_WSV(backend);
    wsv_->add_account(0, "Ivan");
    wsv_->add_domain(0, "RU", 0);
    wsv_->add_asset(0, "USD", 0);
    wsv_->add_balance(0, 0, 100);

    std::string server_address("0.0.0.0:50051");
    QueryServiceImpl service(backend);

    grpc::ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);
    auto server = builder.BuildAndStart();

    auto stub = iroha::Query::NewStub(grpc::CreateChannel(
      "localhost:50051", grpc::InsecureChannelCredentials()));
    {
      iroha::AccountRequest request;
      request.set_account_id(0);
      iroha::AccountReply reply;
      grpc::ClientContext context;
      auto status = stub->GetAccount(&context, request, &reply);
      ASSERT_TRUE(status.ok());
      ASSERT_EQ(reply.name(), "Ivan");
    }
    {
      iroha::BalanceRequest request;
      request.set_account_id(0);
      request.set_asset_id(0);
      iroha::BalanceReply reply;
      grpc::ClientContext context;
      auto status = stub->GetBalance(&context, request, &reply);
      ASSERT_TRUE(status.ok());
      ASSERT_EQ(reply.amount(), 100);
    }

    wsv_->clear();
  }
}
}
