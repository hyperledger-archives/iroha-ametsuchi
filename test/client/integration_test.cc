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
#include <gtest/gtest.h>

TEST(integration_test, sample_test) {
  ametsuchi::Client client;
  {
    auto block = client.block();
    auto txs = block->add_txs();
    auto actions = txs->add_actions();
    auto add_account = actions->mutable_add_account();
    add_account->set_account_id(0);
    add_account->set_name("Ivan");
    ASSERT_EQ(client.append(), 1);
  }
  ASSERT_EQ(client.get_account_by_id(0), "Ivan");
  {
    auto block = client.block();
    auto txs = block->add_txs();
    auto actions = txs->add_actions();
    auto add_domain = actions->mutable_add_domain();
    add_domain->set_domain_id(0);
    add_domain->set_name("RU");
    add_domain->set_root_account_id(0);
    actions = txs->add_actions();
    auto add_asset = actions->mutable_add_asset();
    add_asset->set_asset_id(0);
    add_asset->set_domain_id(0);
    add_asset->set_name("USD");
    ASSERT_EQ(client.append(), 2);
  }
  {
    auto block = client.block();
    auto txs = block->add_txs();
    auto actions = txs->add_actions();
    auto add_balance = actions->mutable_add_balance();
    add_balance->set_account_id(0);
    add_balance->set_asset_id(0);
    add_balance->set_amount(100);
    ASSERT_EQ(client.append(), 3);
  }
  ASSERT_EQ(client.get_balance_by_account_id_asset_id(0, 0), 100);
}
