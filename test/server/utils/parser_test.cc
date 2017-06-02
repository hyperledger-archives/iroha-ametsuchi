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

#include <block_parser.h>
#include <gtest/gtest.h>

#include <action_parser.h>
#include <transaction_parser.h>


TEST(parser_test, join_test) {
  std::string raw_block = "2345234#1_table1 4 5#1_table1 3 4";
  utils::BlockParser blockParser(raw_block);
  ASSERT_EQ("2345234", blockParser.get_hash());
  for (auto tx_raw : blockParser.get_transactions()) {
    utils::TransactionParser transactionParser(tx_raw);
    ASSERT_EQ("1", transactionParser.get_hash());
    for (auto action_raw : transactionParser.get_actions()) {
      utils::ActionParser actionParser(action_raw);
      ASSERT_EQ("table1", actionParser.get_table_name());
    }
  }
}