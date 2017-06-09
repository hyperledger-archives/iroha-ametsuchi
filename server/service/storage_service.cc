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

#include "storage_service.h"

namespace service {

grpc::Status StorageServiceImpl::GetAccount(
    ::grpc::ServerContext *context, const ::iroha::AccountRequest *request,
    ::iroha::AccountResponse *response) {
  auto name = wsv_->get_account_by_id(request->account_id());
  response->set_name(name);
  return grpc::Status::OK;
}
grpc::Status StorageServiceImpl::GetBalance(
    ::grpc::ServerContext *context, const ::iroha::BalanceRequest *request,
    ::iroha::BalanceResponse *response) {
  auto amount = wsv_->get_balance_by_account_id_asset_id(request->account_id(),
                                                         request->asset_id());
  response->set_amount(amount);
  return grpc::Status::OK;
}
grpc::Status StorageServiceImpl::Append(::grpc::ServerContext *context,
                                        const ::iroha::AppendRequest *request,
                                        ::iroha::AppendResponse *response) {
  std::vector<uint8_t> block(request->block().ByteSize());
  request->block().SerializeToArray(block.data(), block.size());
  block_store_->add(request->block().height(), block);
  response->set_id(request->block().height());
  for (auto tx : request->block().txs()) {
    for (auto action : tx.actions()) {
      auto handler = handler_.at((uint8_t)action.command_case());
      handler(action);
    }
  }
  return grpc::Status::OK;
}
}
