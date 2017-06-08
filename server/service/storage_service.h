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

#ifndef AMETSUCHI_STORAGE_SERVICE_H
#define AMETSUCHI_STORAGE_SERVICE_H

#include <block_store_flat.h>
#include <manager.h>
#include <storage.grpc.pb.h>
#include <wsv.h>
#include <unordered_map>
namespace service {
class StorageServiceImpl final : public iroha::Storage::Service {
 public:
  StorageServiceImpl(std::string backend = "Postgres") {
    block_store_.reset(new block_store::BlockStoreFlat);
    auto envbackend = std::getenv("AMWSVBACKEND");
    if (envbackend) {
      backend = envbackend;
    }
    wsv_ = wsv::Manager::instance().make_WSV(backend);
    handler_.insert(std::make_pair(iroha::Action::CommandCase::kAddAccount,
                                   [this](const iroha::Action &action) {
                                     auto add_account = action.add_account();
                                     wsv_->add_account(add_account.account_id(),
                                                       add_account.name());
                                   }));
    handler_.insert(std::make_pair(iroha::Action::CommandCase::kAddAsset,
                                   [this](const iroha::Action &action) {
                                     auto add_asset = action.add_asset();
                                     wsv_->add_asset(add_asset.asset_id(),
                                                     add_asset.name(),
                                                     add_asset.domain_id());
                                   }));
    handler_.insert(std::make_pair(iroha::Action::CommandCase::kAddBalance,
                                   [this](const iroha::Action &action) {
                                     auto add_balance = action.add_balance();
                                     wsv_->add_balance(add_balance.account_id(),
                                                       add_balance.asset_id(),
                                                       add_balance.amount());
                                   }));
    handler_.insert(std::make_pair(
        iroha::Action::CommandCase::kAddDomain,
        [this](const iroha::Action &action) {
          auto add_domain = action.add_domain();
          wsv_->add_domain(add_domain.domain_id(), add_domain.name(),
                           add_domain.root_account_id());
        }));
  }
  grpc::Status GetAccount(::grpc::ServerContext *context,
                          const ::iroha::AccountRequest *request,
                          ::iroha::AccountResponse *response) override;
  grpc::Status GetBalance(::grpc::ServerContext *context,
                          const ::iroha::BalanceRequest *request,
                          ::iroha::BalanceResponse *response) override;

  grpc::Status Append(::grpc::ServerContext *context,
                      const ::iroha::AppendRequest *request,
                      ::iroha::AppendResponse *response) override;

 private:
  std::unique_ptr<block_store::BlockStore> block_store_;
  std::unique_ptr<wsv::WSV> wsv_;
  std::unordered_map<uint8_t, std::function<void(iroha::Action)>> handler_;
};
}

#endif  // AMETSUCHI_STORAGE_SERVICE_H
