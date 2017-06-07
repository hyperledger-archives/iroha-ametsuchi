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

namespace ametsuchi {

Client::Client(){
  auto channel = grpc::CreateChannel(
    "localhost:50051", grpc::InsecureChannelCredentials());
  stub_ = iroha::Storage::NewStub(channel);
}
std::string Client::get_account_by_id(uint64_t account_id) {
  iroha::AccountRequest request;
  request.set_account_id(account_id);
  iroha::AccountResponse response;
  grpc::ClientContext context;
  auto status = stub_->GetAccount(&context, request, &response);
  if (status.ok()) {
    return response.name();
  }
  else {
    return "";
  }
}
uint64_t Client::get_balance_by_account_id_asset_id(uint64_t account_id,
                                                    uint64_t asset_id) {
  iroha::BalanceRequest request;
  request.set_account_id(account_id);
  request.set_asset_id(asset_id);
  iroha::BalanceResponse response;
  grpc::ClientContext context;
  auto status = stub_->GetBalance(&context, request, &response);
  if (status.ok()) {
    return response.amount();
  }
}
uint64_t Client::append() {
  if (!request_){
    // TODO handle
  }
  iroha::AppendResponse response;
  grpc::ClientContext context;
  auto status = stub_->Append(&context, *request_, &response);
  request_.reset(nullptr);
  if (status.ok()){
    return response.id();
  }
}
iroha::Block *Client::block() {
  request_.reset(new iroha::AppendRequest);
  return request_->mutable_block();
}
}
