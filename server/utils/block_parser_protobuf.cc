//
// Created by dainfos on 01.06.17.
//

#include "block_parser_protobuf.h"

namespace ametsuchi {
namespace utils {

BlockParserProtobuf::BlockParserProtobuf(std::vector<uint8_t> blob) {
  block_.ParseFromArray(blob.data(), blob.size());
  block_.prev_hash().data();
}
BlockParserProtobuf::BlockParserProtobuf(std::string filename) {}
google::protobuf::RepeatedPtrField<iroha::Transaction>
BlockParserProtobuf::get_txs() {
  return block_.txs();
}
google::protobuf::RepeatedPtrField<iroha::Signature>
BlockParserProtobuf::get_peer_sigs() {
  return block_.peer_sigs();
}
std::array<uint8_t, 32> BlockParserProtobuf::get_prev_hash() {
  std::array<uint8_t, 32> res;
  auto& str_hash = block_.prev_hash();
  std::copy_n(std::make_move_iterator(str_hash.begin()), 32, res.begin());
  return res;
}
uint32_t BlockParserProtobuf::get_length() { return block_.length(); }
std::array<uint8_t, 32> BlockParserProtobuf::get_merkle_root() {
  std::array<uint8_t, 32> res;
  auto& str_merkle_root = block_.merkle_root();
  std::copy_n(std::make_move_iterator(str_merkle_root.begin()), 32,
              res.begin());
  return res;
}
uint64_t BlockParserProtobuf::get_height() { return block_.height(); }
uint64_t BlockParserProtobuf::get_created() { return block_.created(); }
}
}