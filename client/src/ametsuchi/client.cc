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

#include "ametsuchi/client.h"
#include <grpc++/grpc++.h>

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using iroha::AccountRequest;
using iroha::AccountReply;
using iroha::BalanceRequest;
using iroha::BalanceReply;
using iroha::Query;

namespace ametsuchi {

Client::Client()
    : stub_(Query::NewStub(grpc::CreateChannel(
          "localhost:50051", grpc::InsecureChannelCredentials()))) {}
std::string Client::get_account_by_id(uint64_t account_id) {
  AccountRequest request;
  request.set_account_id(account_id);
  AccountReply reply;
  ClientContext context;
  auto status = stub_->GetAccount(&context, request, &reply);
  if (status.ok()) {
    return reply.name();
  }
  else {
    return "";
  }
}
uint64_t Client::get_balance_by_account_id_asset_id(uint64_t account_id,
                                                    uint64_t asset_id) {
  BalanceRequest request;
  request.set_account_id(account_id);
  request.set_asset_id(asset_id);
  BalanceReply reply;
  ClientContext context;
  auto status = stub_->GetBalance(&context, request, &reply);
  if (status.ok()) {
    return reply.amount();
  }
}
}
