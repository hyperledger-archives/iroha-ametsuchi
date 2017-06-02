#pragma once

#include <cstdlib>
#include <string>

namespace block_index {

class BlockIndex {
 public:
  virtual bool add_blockhash_blockid(std::string blockhash, size_t blockid) = 0;
  virtual size_t get_blockid_by_blockhash(std::string hash) = 0;
};
}
