#include <block_index_mediator.h>
#include <block_parser_protobuf.h>
#include <block_store_flat.h>
#include <flat_iterator.h>
#include <iostream>

namespace crash_handler {

BlockIndexMediator::BlockIndexMediator(BlockIndex &block_index,
                                       BlockStore &block_store)
    : block_index_(block_index), block_store_(block_store) {}

bool BlockIndexMediator::validate() {
  if (block_index_.get_last_blockid() == block_store_.last_id()) {
    return true;
  } else {
    auto last_blockid = block_index_.get_last_blockid();

    auto it = block_store_.begin() + last_blockid;
    for (;it < block_store_.end(); it++) {
      auto block_blob = *it;

      std::string hash(32, '\0');
      utils::sha3_256((unsigned char *)&hash.at(0), block_blob.data(), block_blob.size());
      block_index_.add_blockhash_blockid(hash, ++last_blockid);
    }
    return true;
  }
}
}
