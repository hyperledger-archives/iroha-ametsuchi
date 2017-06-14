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

#include "block_index_redis.h"

namespace block_index {

BlockIndexRedis::BlockIndexRedis() {
  auto env = std::getenv("REDISHOST");
  host_ = env ? env : "127.0.0.1";
  env = std::getenv("REDISPORT");
  port_ = env ? std::stoull(env) : 6379;
  client_.connect(host_, port_);
}

BlockIndexRedis::~BlockIndexRedis() { client_.disconnect(); }

bool BlockIndexRedis::add_blockhash_blockid(std::string block_hash,
                                            uint32_t height) {
  bool res;
  client_.set("block:" + block_hash, std::to_string(height), [&res](cpp_redis::reply& reply) { res = reply.ok(); });
  client_.set("last_id", std::to_string(height), [&res](cpp_redis::reply& reply) { res &= reply.ok(); });
  client_.sync_commit();
  return res;
}

uint32_t BlockIndexRedis::get_blockid_by_blockhash(std::string blockhash) {
  uint32_t res;
  client_.get("block:" + blockhash, [&res](cpp_redis::reply& reply) {
    res = std::stoul(reply.as_string());
  });
  client_.sync_commit();
  return res;
}
uint32_t BlockIndexRedis::get_last_blockid() {
  uint32_t res;
  client_.get("last_id", [&res](cpp_redis::reply& reply) {
    res = std::stoul(reply.as_string());
  });
  client_.sync_commit();
  return res;
}
}
