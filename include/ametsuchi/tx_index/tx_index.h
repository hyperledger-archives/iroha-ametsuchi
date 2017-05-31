#pragma once

#include <string>

namespace ametsuchi{
namespace tx_index{

class TxIndex{
 public:
  virtual bool add_txhash_blockhash_txid(std::string txhash,
                                         std::string blockid,
                                         std::string txid) = 0;
  virtual std::string get_txid_by_txhash(std::string txhash) = 0;
  virtual std::string get_blockhash_by_txhash(std::string txhash) = 0;
};

}
}
