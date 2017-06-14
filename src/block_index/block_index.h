#pragma once

#include <cstdlib>
#include <string>
#include <optional.hpp>

namespace block_index {

class BlockIndex {
 public:
  virtual bool add_blockhash_blockid(std::string block_hash, uint32_t height) = 0;
  virtual std::experimental::optional<uint32_t> get_blockid_by_blockhash(std::string hash) = 0;
//  virtual uint32_t get_last_blockid() = 0;
  virtual bool start_multi() = 0;
  virtual bool exec_multi() = 0;
  virtual bool discard_multi() = 0;
};
}
