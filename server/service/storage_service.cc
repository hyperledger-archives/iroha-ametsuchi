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
  response->set_id(block_store_->append(block));
  for (auto tx : request->block().txs()) {
    for (auto action : tx.actions()) {
      if (action.has_add_account()) {
        auto add_account = action.add_account();
        wsv_->add_account(add_account.account_id(), add_account.name());
      } else if (action.has_add_balance()) {
        auto add_balance = action.add_balance();
        wsv_->add_balance(add_balance.account_id(), add_balance.asset_id(),
                          add_balance.amount());
      } else if (action.has_add_domain()) {
        auto add_domain = action.add_domain();
        wsv_->add_domain(add_domain.domain_id(), add_domain.name(),
                         add_domain.root_account_id());
      } else if (action.has_add_asset()) {
        auto add_asset = action.add_asset();
        wsv_->add_asset(add_asset.asset_id(), add_asset.name(),
                        add_asset.domain_id());
      }
    }
  }
  return grpc::Status::OK;
}
}
