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

#include <cpp_redis/redis_client.hpp>
#include "wsv.h"
#include "factory.h"
#include "manager.h"

namespace wsv {

class WSVRedis : public WSV {
 public:
  WSVRedis() {
    auto env = std::getenv("REDISHOST");
    host_ = env ? env : "127.0.0.1";
    env = std::getenv("REDISPORT");
    port_ = env ? std::stoull(env) : 6379;
    client_.connect(host_, port_);
  }
  ~WSVRedis();

  bool add_account(uint64_t account_id, std::string name) override;
  bool add_domain(uint64_t domain_id,
                  std::string name,
                  uint64_t root_account_id) override;
  bool add_asset(uint64_t asset_id,
                 std::string name,
                 uint64_t domain_id) override;
  bool add_balance(uint64_t account_id,
                   uint64_t asset_id,
                   std::uint64_t amount) override;
  std::string get_account_by_id(uint64_t account_id) override;
  uint64_t get_balance_by_account_id_asset_id(uint64_t account_id,
                                              uint64_t asset_id) override;

 private:
  cpp_redis::redis_client client_;
  std::string host_;
  size_t port_;
};

WSVRedis::~WSVRedis() { client_.disconnect(); }

bool WSVRedis::add_account(uint64_t account_id, std::string name) {
  bool res;
  std::string string_id = "account:" + std::to_string(account_id);
  client_.hset(string_id, "name", name, [&res](cpp_redis::reply& reply) { res = reply.ok(); });
  client_.sync_commit();
  return res;
}

std::string WSVRedis::get_account_by_id(uint64_t account_id) {
  std::string res;
  client_.hget("account:" + std::to_string(account_id), "name",
               [&res](cpp_redis::reply& reply) { res = reply.as_string(); });
  client_.sync_commit();
  return res;
}

bool WSVRedis::add_domain(uint64_t domain_id, std::string name,
                          uint64_t root_account_id) {
  bool res;
  std::string string_id = "domain:" + std::to_string(domain_id);
  client_.hset(string_id, "name", name, [&res](cpp_redis::reply& reply) { res = reply.ok(); });
  client_.hset(string_id, "root_account", std::to_string(root_account_id), [&res](cpp_redis::reply& reply) { res &= reply.ok(); });
  client_.sync_commit();
  return true;
}
bool WSVRedis::add_asset(uint64_t asset_id, std::string name,
                         uint64_t domain_id) {
  bool res;
  std::string string_id = "asset:" + std::to_string(asset_id);
  client_.hset(string_id, "name", name, [&res](cpp_redis::reply& reply) { res = reply.ok(); });
  client_.hset(string_id, "domain", std::to_string(domain_id), [&res](cpp_redis::reply& reply) { res &= reply.ok(); });
  client_.sync_commit();
  return res;
}
bool WSVRedis::add_balance(uint64_t account_id, uint64_t asset_id,
                           std::uint64_t amount) {
  bool res;
  std::string string_id = "account:" + std::to_string(account_id) + "asset:" +
                          std::to_string(asset_id);
  //  If field does not exist the value is set to 0 before the operation is
  //  performed.
  client_.hincrby(string_id, "balance", amount,
                  [&res](cpp_redis::reply& reply) { res = reply.ok(); });
  client_.sync_commit();
  return res;
}
uint64_t WSVRedis::get_balance_by_account_id_asset_id(uint64_t account_id,
                                                      uint64_t asset_id) {
  uint64_t res;
  client_.hget("account:" + std::to_string(account_id) + "asset:" +
                   std::to_string(asset_id),
               "balance", [&res](cpp_redis::reply& reply) {
                 res = reply.is_null() ? 0 : std::stoi(reply.as_string());
               });
  client_.sync_commit();
  return res;
}

class RedisFactory : public Factory {
 public:
  RedisFactory(){
    Manager::instance().insert(*this);
  }
  std::string name() const override {
    return "Redis";
  }
  std::unique_ptr<WSV> create_instance() override {
    return std::make_unique<WSVRedis>();
  }
};

static RedisFactory redisFactory;

}
