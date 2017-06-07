#include "block_index_redis.h"

namespace block_index {

BlockIndexRedis::BlockIndexRedis(std::string host, size_t port) : host_(host), port_(port) {
  client_.connect(host, port);

}

BlockIndexRedis::~BlockIndexRedis() { client_.disconnect(); }

bool BlockIndexRedis::add_blockhash_blockid(std::string blockhash,
                                            size_t blockid) {
  client_.set("block:" + blockhash, std::to_string(blockid));
  client_.set("last_id", std::to_string(blockid));
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
size_t BlockIndexRedis::get_last_blockid() {
  size_t res;
  client_.get("last_id", [&res](cpp_redis::reply& reply) {
    res = std::stoul(reply.as_string());
  });
  client_.sync_commit();
  return res;
}
}
