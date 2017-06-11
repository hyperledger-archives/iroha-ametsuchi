#pragma once

#include <cpp_redis/cpp_redis>
#include "block_index.h"

namespace block_index {

class BlockIndexRedis : public BlockIndex {
 public:
  BlockIndexRedis();

  ~BlockIndexRedis();

  bool add_blockhash_blockid(std::string block_hash, uint32_t height);
  uint32_t get_blockid_by_blockhash(std::string blockhash);

  uint32_t get_last_blockid();

 private:
  cpp_redis::redis_client client_;
  std::string host_;
  size_t port_;
};
}
