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


#ifndef AMETSUCHI_AMETSUCHI_H
#define AMETSUCHI_AMETSUCHI_H

#include <ametsuchi/block_index/block_index.h>
#include <ametsuchi/block_store/block_store.h>
#include <ametsuchi/tx_index/tx_index.h>
#include <ametsuchi/wsv/wsv.h>
#include <memory>

namespace ametsuchi {

const std::string block_index_host = "10.90.130.160";
const size_t block_index_port = 6379;

const std::string tx_index_host = "10.90.130.160";
const size_t tx_index_port = 6379;

const std::string wsv_host = "10.90.130.160";
const size_t wsv_port = 6379;

class Ametsuchi {
 public:
  Ametsuchi();
  void append(const std::string tx);

  // Queries on BlockStore
  std::string get_block_by_hash(std::string block_hash);

  // Queries on transactions
  std::string get_transaction_by_hash(std::string tx_hash);

  // Queries on WSV:
  size_t get_balance_by_accountid(size_t account_id);

 private:
  std::unique_ptr<block_store::BlockStore> block_store_;
  std::unique_ptr<block_index::BlockIndex> block_index_;
  std::unique_ptr<tx_index::TxIndex> tx_index_;
  std::unique_ptr<wsv::WSV> wsv_;
};
}

#endif  // AMETSUCHI_AMETSUCHI_H