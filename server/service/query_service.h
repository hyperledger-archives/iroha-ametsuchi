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

#ifndef AMETSUCHI_QUERYSERVICE_H
#define AMETSUCHI_QUERYSERVICE_H

#include <grpc++/grpc++.h>
#include <query.grpc.pb.h>
#include <wsv.h>
#include <manager.h>

namespace service {

class QueryServiceImpl final : public iroha::Query::Service {
 public:
  QueryServiceImpl(std::string backend = "Postgres") {
    auto envbackend = std::getenv("AMWSVBACKEND");
    if (envbackend){
      backend = envbackend;
    }
    wsv_ = wsv::Manager::instance().make_WSV(backend);
  }
  grpc::Status GetAccount(::grpc::ServerContext *context,
                          const ::iroha::AccountRequest *request,
                          ::iroha::AccountResponse *response) override;
  grpc::Status GetBalance(::grpc::ServerContext *context,
                    const ::iroha::BalanceRequest *request,
                    ::iroha::BalanceResponse *response) override;

 private:
  std::unique_ptr<wsv::WSV> wsv_;
};

}
#endif //AMETSUCHI_QUERYSERVICE_H
