#pragma once

#include <cpp_redis/cpp_redis>
#include "block_index.h"

namespace block_index {

class BlockIndexRedis : public BlockIndex {
 public:
  BlockIndexRedis();

  ~BlockIndexRedis();

  bool add_blockhash_blockid(std::string block_hash, uint32_t height);
  std::experimental::optional<uint32_t> get_blockid_by_blockhash(
      std::string blockhash);

  //  uint32_t get_last_blockid();
  bool start_multi() override;
  bool exec_multi() override;
  bool discard_multi() override;

 private:
  cpp_redis::redis_client client_, read_client_;
  std::string host_;
  size_t port_;
};
}
