#pragma once

#include <string>

namespace ametsuchi{
namespace tx_index{

class TxIndex{
 public:
  virtual bool add_txhash_blockhash_txid(std::string txhash,
                                         size_t blockid,
                                         size_t txid) = 0;
  virtual size_t get_txid_by_txhash(std::string txhash) = 0;
  virtual merkle_tree::hash_t get_blockhash_by_txhash(std::string txhash) = 0;
};

}
}
