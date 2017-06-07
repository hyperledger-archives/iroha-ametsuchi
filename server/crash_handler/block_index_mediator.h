//
// Created by kamilsa on 06.06.17.
//

#ifndef AMETSUCHI_BLOCK_INDEX_MEDIATOR_H
#define AMETSUCHI_BLOCK_INDEX_MEDIATOR_H

#include <block_index.h>
#include <block_store.h>
#include <hash.h>

namespace crash_handler{

using block_index::BlockIndex;
using block_store::BlockStore;

class BlockIndexMediator{
 public:
  BlockIndexMediator(BlockIndex& block_index, BlockStore& block_store);
  bool validate();
 private:
  BlockIndex& block_index_;
  BlockStore& block_store_;
};

}

#endif //AMETSUCHI_BLOCK_INDEX_MEDIATOR_H
