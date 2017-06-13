#pragma once

#include <string>
#include <vector>

namespace tx_index {

class TxIndex {
 public:
  virtual bool add_txhash_blockhash_txid(std::string txhash,
                                         uint32_t height,
                                         int txid) = 0;
  virtual int get_txid_by_txhash(std::string txhash) = 0;
  virtual std::string get_blockhash_by_txhash(std::string txhash) = 0;

  virtual bool add_block(std::vector<uint8_t > block_blob) = 0;
  virtual size_t get_last_blockid() = 0;
};
}
