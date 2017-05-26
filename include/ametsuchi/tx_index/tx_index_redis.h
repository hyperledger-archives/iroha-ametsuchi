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

#pragma once

#include <cpp_redis/redis_client.hpp>
#include <ametsuchi/merkle_tree/merkle_tree.h>
#include "tx_index.h"

namespace ametsuchi {

namespace tx_index{

class TxIndexRedis: public TxIndex{
 public:
  TxIndexRedis(std::string host, size_t port): host_(host), port_(port){
    client_.connect(host, port);
  }

  bool add_txhash_blockhash_txid(std::string txhash,
                                 size_t blockid,
                                 size_t txid);
  size_t get_txid_by_txhash(std::string txhash);
  merkle_tree::hash_t get_blockhash_by_txhash(std::string txhash);

  ~TxIndexRedis();

 private:
  cpp_redis::redis_client client_;
  std::string host_;
  size_t port_;
};

}

}

