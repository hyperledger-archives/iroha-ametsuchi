#pragma once

#include <string>

namespace tx_index {

class TxIndex {
 public:
  virtual bool add_txhash_blockhash_txid(std::string txhash,
                                         uint32_t height,
                                         int txid) = 0;
  virtual int get_txid_by_txhash(std::string txhash) = 0;
  virtual std::string get_blockhash_by_txhash(std::string txhash) = 0;
};
}
