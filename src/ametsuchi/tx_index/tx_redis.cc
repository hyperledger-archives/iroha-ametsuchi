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

#include <ametsuchi/tx_index/tx_index_redis.h>
namespace ametsuchi {

namespace tx_index {

TXRedis::~TXRedis() {
  client_.disconnect();
}

bool TXRedis::add_id(std::string hash, size_t id) {
  std::string string_id = "hash:" + hash;
  client_.set(string_id, std::to_string(id));
  client_.sync_commit();
}

bool TXRedis::add_timestamp(size_t id, size_t timestamp) {
  std::string string_id = "id" + std::to_string(id);
  std::vector <std::string> arr;
  client_.sadd(string_id,
               std::vector < std::string > {std::to_string(timestamp)});
  client_.sync_commit();
}

size_t TXRedis::get_id_by_hash(std::string hash) {
  size_t res;
  client_.get("hash:" + hash,
              [&res](cpp_redis::reply &reply) {
                res = std::stoul(reply.as_string());
              });
  client_.sync_commit();
  return res;
}

std::vector <size_t> TXRedis::get_ids_by_timestamp(size_t timestamp) {
  std::vector <cpp_redis::reply> replies;
  client_.smembers("id:" + std::to_string(timestamp),
                   [&replies](cpp_redis::reply &reply) {
                     replies = reply.as_array();
                   });
  std::vector <size_t> res;
  for (auto i = replies.begin(); i < replies.end(); ++i) {
    res.push_back(std::stoul((*i).as_string()));
  }
  return res;
}

}
}
