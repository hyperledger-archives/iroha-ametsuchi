#pragma once

#include <string>

namespace ametsuchi{
namespace tx_index{

class TxIndex{
 public:
  virtual bool add_txhash_blockid_txid(std::string txhash, size_t blockid, size_t txid) = 0;
  virtual size_t get_txid_by_txhash(std::string txhash) = 0;
  virtual size_t get_blockid_by_txhash(std::string txhash) = 0;
};

}
}