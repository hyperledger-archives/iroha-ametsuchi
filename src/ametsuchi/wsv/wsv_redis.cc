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

#include <ametsuchi/wsv/wsv_redis.h>

namespace ametsuchi {

namespace wsv {

WSVRedis::~WSVRedis() { client_.disconnect(); }

bool WSVRedis::add_account(uint64_t account_id, std::string name) {
  std::string string_id = "account:" + std::to_string(account_id);
  client_.hset(string_id, "name", name);
  client_.hset(string_id, "balance", "0");
  client_.sync_commit();
  return true;
}

bool WSVRedis::add_balance(uint64_t account_id, std::uint64_t amount) {
  std::string string_id = "account:" + std::to_string(account_id);
  client_.hincrby(string_id, "balance", amount);
  client_.sync_commit();
  return true;
}

std::string WSVRedis::get_account_by_id(uint64_t account_id) {
  std::string res;
  client_.hget("account:" + std::to_string(account_id), "name",
               [&res](cpp_redis::reply& reply) { res = reply.as_string(); });
  client_.sync_commit();
  return res;
}

uint64_t WSVRedis::get_balance_by_account_id(uint64_t account_id) {
  uint64_t res;
  client_.hget(
      "account:" + std::to_string(account_id), "balance",
      [&res](cpp_redis::reply& reply) { res = std::stoi(reply.as_string()); });
  client_.sync_commit();
  return res;
}

void WSVRedis::flush_all() { client_.flushall(); }
}
}
