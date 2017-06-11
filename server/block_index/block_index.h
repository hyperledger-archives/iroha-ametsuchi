#pragma once

#include <cstdlib>
#include <string>

namespace block_index {

class BlockIndex {
 public:
  virtual bool add_blockhash_blockid(std::string block_hash, uint32_t height) = 0;
  virtual uint32_t get_blockid_by_blockhash(std::string hash) = 0;
  virtual uint32_t get_last_blockid() = 0;
};
}
