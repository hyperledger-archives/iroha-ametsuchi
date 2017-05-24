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
#include <wsv/wsv_redis.h>

TEST(WSV_TEST, GET_TEST){
  wsv::WSVRedis wsvRedis("127.0.0.1", 6379);

  wsvRedis.add_account(1, "Ivan");

  std::string ivan = wsvRedis.get_account_by_id(1);

  ASSERT_EQ(ivan, "Ivan");

  uint64_t balance = wsvRedis.get_balance_by_account_id(1);
  ASSERT_EQ(balance, 0);
//
  wsvRedis.add_balance(1, 100);
  balance = wsvRedis.get_balance_by_account_id(1);
  ASSERT_EQ(balance, 100);

  wsvRedis.flush_all();
}