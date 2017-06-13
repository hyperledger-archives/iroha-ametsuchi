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

#include "tx_index_redis.h"
//#include <block_parser_protobuf.h>
#include <hash.h>
#include <block.pb.h>

namespace tx_index {

TxIndexRedis::~TxIndexRedis() { client_.disconnect(); }

bool TxIndexRedis::add_txhash_blockhash_txid(std::string txhash,
                                             uint32_t height,
                                             int txid) {
  bool res = _add_txhash_blockhash_txid(txhash, height, txid);
  client_.sync_commit();
  return res;
}

int TxIndexRedis::get_txid_by_txhash(std::string txhash) {
  int res;
  client_.hget("tx:" + txhash, "txid",
               [&res](cpp_redis::reply& reply) { res = std::stoi(reply.as_string()); });
  client_.sync_commit();
  return res;
}

std::string TxIndexRedis::get_blockhash_by_txhash(std::string txhash) {
  std::string res;
  client_.hget("tx:" + txhash, "blockid",
               [&res](cpp_redis::reply& reply) { res = reply.as_string(); });
  client_.sync_commit();
  return res;
}


TxIndexRedis::TxIndexRedis() {
  auto env = std::getenv("REDISHOST");
  host_ = env ? env : "127.0.0.1";
  env = std::getenv("REDISPORT");
  port_ = env ? std::stoull(env) : 6379;
  client_.connect(host_, port_);
}

bool TxIndexRedis::add_block(std::vector<uint8_t> block_blob) {
//  utils::BlockParserProtobuf parser(block_blob);
  iroha::Block block;
  block.ParseFromArray(block_blob.data(), block_blob.size());
//  auto txs = parser.get_txs();
  auto txs = block.body().transaction();

  int txid = 0;
  for (const auto& tx : txs){
    const auto& body = tx.body();
    std::string tx_hash(32, '\0');
    const unsigned char* body_bytes = nullptr;

    body.SerializeToArray((void *) body_bytes, body.ByteSize());
    utils::sha3_256((unsigned char *) &tx_hash.at(0), body_bytes, body.ByteSize());

//    _add_txhash_blockhash_txid(tx_hash, parser.get_height(), txid++);
    _add_txhash_blockhash_txid(tx_hash, block.meta().height(), txid++);
  }
  client_.sync_commit();
  return true;
}

size_t TxIndexRedis::get_last_blockid() {
  size_t res;
  client_.get("block_id_last",
               [&res](cpp_redis::reply& reply) { res = std::stoul(reply.as_string()); });
  client_.sync_commit();
  return res;
}

bool TxIndexRedis::_add_txhash_blockhash_txid(std::string txhash, uint32_t height, int txid) {
  bool res;
  client_.hset("tx:" + txhash, "blockid", std::to_string(height), [&res](cpp_redis::reply& reply) { res = reply.ok(); });
  client_.hset("tx:" + txhash, "txid", std::to_string(txid), [&res](cpp_redis::reply& reply) { res &= reply.ok(); });
  return true;
}

}
