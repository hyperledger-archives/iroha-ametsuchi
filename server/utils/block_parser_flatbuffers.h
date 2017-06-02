//
// Created by dainfos on 31.05.17.
//

#ifndef AMETSUCHI_BLOCK_PARSER_FLATBUFFERS_H
#define AMETSUCHI_BLOCK_PARSER_FLATBUFFERS_H

#include <block_generated.h>
#include <flatbuffers/flatbuffers.h>
#include <string>
#include <vector>

namespace utils {

class BlockParserFlatbuffers {
 public:
  BlockParserFlatbuffers(std::vector<uint8_t> blob);
  std::vector<uint8_t> get_txs();
  std::vector<uint8_t> get_peer_sigs();
  std::array<uint8_t, 32> get_prev_hash();
  uint32_t get_length();
  std::array<uint8_t, 32> get_merkle_root();
  uint64_t get_height();
  std::array<uint8_t, 25> get_created();

 private:
  const iroha::Block* block_;

  std::vector<uint8_t> txs_;
  std::vector<uint8_t> peer_sigs_;
  std::array<uint8_t, 32> prev_hash_;
  uint32_t length_;
  std::array<uint8_t, 32> merkle_root_;
  uint64_t height_;
  std::array<uint8_t, 25> created_;

  std::vector<uint8_t> _get_txs();
  std::vector<uint8_t> _get_peer_sigs();
  std::array<uint8_t, 32> _get_prev_hash();
  uint32_t _get_length();
  std::array<uint8_t, 32> _get_merkle_root();
  uint64_t _get_height();
  std::array<uint8_t, 25> _get_created();
};
}

#endif  // AMETSUCHI_BLOCK_PARSER_FLATBUFFERS_H
