#include "block_index_redis.h"

namespace block_index {

BlockIndexRedis::~BlockIndexRedis() { client_.disconnect(); }

bool BlockIndexRedis::add_blockhash_blockid(std::string blockhash,
                                            size_t blockid) {
  client_.set("block:" + blockhash, std::to_string(blockid));
  client_.sync_commit();
  return true;
}

size_t BlockIndexRedis::get_blockid_by_blockhash(std::string blockhash) {
  size_t res;
  client_.get("block:" + blockhash, [&res](cpp_redis::reply& reply) {
    res = std::stoul(reply.as_string());
  });
  client_.sync_commit();
  return res;
}
}
