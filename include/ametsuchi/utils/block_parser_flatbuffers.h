//
// Created by dainfos on 31.05.17.
//

#ifndef AMETSUCHI_BLOCK_PARSER_FLATBUFFERS_H
#define AMETSUCHI_BLOCK_PARSER_FLATBUFFERS_H

#include <vector>
#include <string>
#include <flatbuffers/flatbuffers.h>

namespace ametsuchi{
namespace utils{

class BlockParserFlatbuffers{
 public:
  BlockParserFlatbuffers(std::vector<uint8_t> blob);
  std::vector<uint8_t > get_txs();
  std::vector<uint8_t > get_peer_sigs();
  std::array<uint8_t, 32> get_prev_hash();
  uint32_t get_length();
  std::vector<uint8_t > get_merkle_root();
  uint64_t get_height();
  std::array<uint8_t, 25> get_timestamp();
 private:
  flatbuffers::FlatBufferBuilder builder;
};

}
}

#endif //AMETSUCHI_BLOCK_PARSER_FLATBUFFERS_H
