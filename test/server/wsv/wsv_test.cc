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
#include <wsv.h>
#include <manager.h>

TEST(wsv_test, sample_test) {
  for (auto backend: {"Postgres", "Redis"}){
    auto wsv_ = wsv::Manager::instance().make_WSV(backend);
    wsv_->add_account(0, "Ivan");
    ASSERT_EQ(wsv_->get_account_by_id(0), "Ivan");
    wsv_->add_domain(0, "RU", 0);
    wsv_->add_asset(0, "USD", 0);
    ASSERT_EQ(wsv_->get_balance_by_account_id_asset_id(0, 0), 0);
    wsv_->add_balance(0, 0, 100);
    ASSERT_EQ(wsv_->get_balance_by_account_id_asset_id(0, 0), 100);
    wsv_->add_balance(0, 0, 100);
    ASSERT_EQ(wsv_->get_balance_by_account_id_asset_id(0, 0), 200);
    wsv_->clear();
  }
}
