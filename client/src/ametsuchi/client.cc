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
using iroha::AccountResponse;
using iroha::BalanceRequest;
using iroha::BalanceResponse;
using iroha::Query;
using iroha::AppendRequest;
using iroha::AppendResponse;
using iroha::Command;

namespace ametsuchi {

Client::Client(){
  auto channel = grpc::CreateChannel(
    "localhost:50051", grpc::InsecureChannelCredentials());
  query_stub_ = Query::NewStub(channel);
  command_stub_ = Command::NewStub(channel);
}
std::string Client::get_account_by_id(uint64_t account_id) {
  AccountRequest request;
  request.set_account_id(account_id);
  AccountResponse response;
  ClientContext context;
  auto status = query_stub_->GetAccount(&context, request, &response);
  if (status.ok()) {
    return response.name();
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
  BalanceResponse response;
  ClientContext context;
  auto status = query_stub_->GetBalance(&context, request, &response);
  if (status.ok()) {
    return response.amount();
  }
}
uint64_t Client::append(iroha::Block *block) {
  AppendRequest request;
  request.set_allocated_block(block);
  AppendResponse response;
  ClientContext context;
  auto status = command_stub_->Append(&context, request, &response);
  if (status.ok()){
    return response.id();
  }
}
}
