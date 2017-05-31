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

TxIndexRedis::~TxIndexRedis() {
  client_.disconnect();
}

bool TxIndexRedis::add_txhash_blockhash_txid(std::string txhash,
                                             std::string blockid,
                                             std::string txid) {
  client_.hset("tx:"+txhash, "blockid", blockid);
  client_.hset("tx:"+txhash, "txid", txid);
  client_.sync_commit();
}

std::string TxIndexRedis::get_txid_by_txhash(std::string txhash) {
  std::string res;
  client_.hget("tx:"+txhash, "txid", [&res](cpp_redis::reply& reply) { res = reply.as_string(); });
  client_.sync_commit();
  return res;
}

std::string TxIndexRedis::get_blockhash_by_txhash(std::string txhash) {
  std::string res;
  client_.hget("tx:"+txhash, "blockid", [&res](cpp_redis::reply& reply) { res = reply.as_string(); });
  client_.sync_commit();
  return res;
}

}
}
