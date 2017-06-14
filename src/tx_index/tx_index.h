#pragma once

#include <optional.hpp>
#include <string>
#include <vector>

namespace tx_index {

class TxIndex {
 public:
  virtual bool add_txhash_blockid_txid(std::string txhash, uint32_t height,
                                       int txid) = 0;
  virtual std::experimental::optional<int> get_txid_by_txhash(
      std::string txhash) = 0;
  virtual std::experimental::optional<uint32_t> get_blockid_by_txhash(
      std::string txhash) = 0;

  virtual bool start_multi() = 0;
  virtual bool exec_multi() = 0;
  virtual bool discard_multi() = 0;

  //  virtual bool add_block(std::vector<uint8_t > block_blob) = 0;
  //  virtual size_t get_last_blockid() = 0;
};
}
