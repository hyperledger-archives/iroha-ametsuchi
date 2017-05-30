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

#include <ametsuchi/ametsuchi.h>
#include <ametsuchi/block_index/block_index_redis.h>
#include <ametsuchi/block_store/block_store_nudb.h>
#include <ametsuchi/utils/action_parser.h>
#include <ametsuchi/utils/block_parser.h>
#include <ametsuchi/utils/transaction_parser.h>
#include <ametsuchi/wsv/wsv_redis.h>
#include <ametsuchi/tx_index/tx_index_redis.h>

namespace ametsuchi {

Ametsuchi::Ametsuchi()
    : block_store_(new block_store::BlockStoreNuDB(block_store_path)),
      block_index_(
          new block_index::BlockIndexRedis(block_index_host, block_index_port)),
      tx_index_(new tx_index::TxIndexRedis(tx_index_host, tx_index_port)),
      wsv_(new wsv::WSVRedis(wsv_host, wsv_port)) {}

void Ametsuchi::append(const std::string block) {
  // Block store append
  utils::BlockParser block_parser(block);
  //block_store_->append(block_parser.get_hash(), block);

  // Block Index - pass meta and block

  // for each Transaction
  auto transactions = block_parser.get_transactions();
  for (size_t tx_id = 0; tx_id < transactions.size(); tx_id++) {
    // do Transaction parser - return meta and vector of strings (Actions)
    utils::TransactionParser transaction_parser(transactions.at(tx_id));
    // Append  meta of transaction and transaction blob to tx index
    tx_index_->add_txhash_blockhash_txid(transaction_parser.get_hash(),
                                         block_parser.get_hash(),
                                         tx_id);
    // Send to WSV [Actions]
    auto actions = transaction_parser.get_actions();
    // For each action in Actions :
    for (auto raw_action : actions) {
      // Parse Action - get table name, key(account_id), value(name, balance)
      // Put to wsv
      utils::ActionParser action_parser(raw_action);
      if (action_parser.get_table_name() == "account") {
        wsv_->add_account(std::stoul(action_parser.get_key()),
                          action_parser.get_value());
      } else if (action_parser.get_table_name() == "balance") {
        wsv_->add_balance(std::stoul(action_parser.get_key()),
                          std::stoul(action_parser.get_value()));
      }
    }
  }
}

std::string Ametsuchi::get_block_by_hash(merkle_tree::hash_t block_hash) {
  return block_store_->get(block_hash);
}

std::string Ametsuchi::get_transaction_by_hash(std::string tx_hash) {
  auto block_hash = tx_index_->get_blockhash_by_txhash(tx_hash);
  auto tx_id = tx_index_->get_txid_by_txhash(tx_hash);

  auto block = block_store_->get(block_hash);
  utils::BlockParser block_parser(block);
  return block_parser.get_transactions().at(tx_id);
}

size_t Ametsuchi::get_balance_by_accountid(size_t account_id) {
  return wsv_->get_balance_by_account_id(account_id);
}
}
