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
#include "tx_index.h"

namespace tx_index {

class TxIndexRedis : public TxIndex {
 public:
  TxIndexRedis();

  bool add_txhash_blockhash_txid(std::string txhash, uint32_t height,
                                 int txid) override;
  int get_txid_by_txhash(std::string txhash) override;
  std::string get_blockhash_by_txhash(std::string txhash) override;

//  bool add_block(std::vector<uint8_t > block_blob) override;
  size_t get_last_blockid() override;

  ~TxIndexRedis();

 private:
  cpp_redis::redis_client client_;
  std::string host_;
  size_t port_;

  // addition to the tx index without committing
  bool _add_txhash_blockhash_txid(std::string txhash, uint32_t height, int txid);
};
}
