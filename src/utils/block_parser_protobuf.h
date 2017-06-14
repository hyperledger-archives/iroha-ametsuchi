//
// Created by dainfos on 01.06.17.
//

#ifndef AMETSUCHI_BLOCK_PARSER_PROTOBUF_H
#define AMETSUCHI_BLOCK_PARSER_PROTOBUF_H

//#include <block.pb.h>
#include <array>
#include <cstdint>
#include <string>
#include <vector>


namespace utils {

class BlockParserProtobuf {
 public:
  BlockParserProtobuf(std::vector<uint8_t> blob);
  BlockParserProtobuf(std::string filename);
  google::protobuf::RepeatedPtrField<iroha::Transaction> get_txs();
  google::protobuf::RepeatedPtrField<iroha::Signature> get_peer_sigs();
  std::array<uint8_t, 32> get_prev_hash();
  uint32_t get_length();
  std::array<uint8_t, 32> get_merkle_root();
  uint64_t get_height();
  uint64_t get_created();

 private:
  iroha::Block block_;
};
}

#endif  // AMETSUCHI_BLOCK_PARSER_PROTOBUF_H
