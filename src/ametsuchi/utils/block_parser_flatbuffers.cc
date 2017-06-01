#include <ametsuchi/utils/block_parser_flatbuffers.h>

namespace ametsuchi {
namespace utils {

BlockParserFlatbuffers::BlockParserFlatbuffers(std::vector<uint8_t> blob)
    : block_(flatbuffers::GetRoot<iroha::Block>(blob.data())),
      txs_(_get_txs()),
      peer_sigs_(_get_peer_sigs()),
      prev_hash_(_get_prev_hash()),
      length_(_get_length()),
      merkle_root_(_get_merkle_root()),
      height_(_get_height()),
      created_(_get_created()) {}

std::vector<uint8_t> BlockParserFlatbuffers::_get_txs() {
  auto txs = block_->txs();
  std::vector<uint8_t> res{txs->Data(), txs->Data() + txs->size()};
  return res;
}
std::vector<uint8_t> BlockParserFlatbuffers::_get_peer_sigs() {
  auto peer_sigs = block_->peer_sigs();
  std::vector<uint8_t> res{peer_sigs->Data(),
                           peer_sigs->Data() + peer_sigs->size()};
  return res;
}
std::array<uint8_t, 32> BlockParserFlatbuffers::_get_prev_hash() {
  auto prev_hash = block_->prev_hash();
  std::array<uint8_t, 32> res;
  std::copy_n(std::make_move_iterator(prev_hash->begin()), 32, res.begin());
  return res;
}
uint32_t BlockParserFlatbuffers::_get_length() { return block_->length(); }
std::array<uint8_t, 32> BlockParserFlatbuffers::_get_merkle_root() {
  auto merkle_root = block_->merkle_root();
  std::array<uint8_t, 32> res;
  std::copy_n(std::make_move_iterator(merkle_root->begin()), 32, res.begin());
  return res;
}
uint64_t BlockParserFlatbuffers::_get_height() { return block_->height(); }
std::array<uint8_t, 25> BlockParserFlatbuffers::_get_created() {
  auto prev_hash = block_->created();
  std::array<uint8_t, 25> res;
  std::copy_n(std::make_move_iterator(prev_hash->begin()), 25, res.begin());
  return res;
}
std::vector<uint8_t> BlockParserFlatbuffers::get_txs() { return txs_; }
std::vector<uint8_t> BlockParserFlatbuffers::get_peer_sigs() {
  return peer_sigs_;
}
std::array<uint8_t, 32> BlockParserFlatbuffers::get_prev_hash() {
  return prev_hash_;
}
uint32_t BlockParserFlatbuffers::get_length() {
  return length_;
}
std::array<uint8_t, 32> BlockParserFlatbuffers::get_merkle_root() {
  return merkle_root_;
}
uint64_t BlockParserFlatbuffers::get_height() {
  return height_;
}
std::array<uint8_t, 25> BlockParserFlatbuffers::get_created() {
  return created_;
}

}
}
