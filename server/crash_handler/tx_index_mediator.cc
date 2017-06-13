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

#include <tx_index_mediator.h>
#include <hash.h>
#include <block_parser_protobuf.h>

crash_handler::TxIndexMediator::TxIndexMediator(TxIndex &tx_index, BlockStore &block_store) : tx_index_(tx_index), block_store_(block_store){}

bool crash_handler::TxIndexMediator::validate() {
  if (tx_index_.get_last_blockid() == block_store_.last_id()){
    return true;
  }
  else{
    auto last_blockid = tx_index_.get_last_blockid();
    auto it = block_store_.begin() + last_blockid;

    for (;it < block_store_.end(); it++) {
      auto block_blob = *it;
      tx_index_.add_block(block_blob);
    }
    return true;
  }
}

