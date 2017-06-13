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

#ifndef AMETSUCHI_TX_INDEX_MEDIATOR_H
#define AMETSUCHI_TX_INDEX_MEDIATOR_H

#include <tx_index.h>
#include <block_store.h>

namespace crash_handler{

using tx_index::TxIndex;
using block_store::BlockStore;

class TxIndexMediator{
 public:
  TxIndexMediator(TxIndex &tx_index, BlockStore &block_store);
  bool validate();

 private:
  TxIndex& tx_index_;
  BlockStore& block_store_;
};

}

#endif //AMETSUCHI_TX_INDEX_MEDIATOR_H
