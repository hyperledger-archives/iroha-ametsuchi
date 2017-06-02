#include <block.pb.h>
#include <block_parser_protobuf.h>
#include <gtest/gtest.h>
#include <fstream>

using ametsuchi::utils::BlockParserProtobuf;

TEST(BLOCK_PARSE_PROTOBUF_TEST, PARSE_TEST) {
  iroha::Block block;
  auto transaction = block.add_txs();
  auto signature = block.add_peer_sigs();

  std::vector<char> hash(32, '0');
  block.set_prev_hash(hash.data());

  block.set_length(250);

  std::vector<char> merkle_root(32, '1');
  block.set_merkle_root(merkle_root.data());

  block.set_height(500);
  block.set_created(1);

  std::string filename = "/tmp/temp";
  std::fstream output(filename,
                      std::ios::out | std::ios::trunc | std::ios::binary);
  block.SerializePartialToOstream(&output);
  output.close();

  BlockParserProtobuf parser(filename);
  ASSERT_EQ(parser.get_length(), 250);
  ASSERT_EQ(parser.get_height(), 500);
  ASSERT_EQ(parser.get_created(), 1);

  std::array<uint8_t, 32> hash_to_compare;
  std::copy_n(std::make_move_iterator(hash.begin()), 32, hash_to_compare.begin());
  ASSERT_EQ(parser.get_prev_hash(), hash_to_compare);

  std::array<uint8_t, 32> merkle_root_to_compare;
  std::copy_n(std::make_move_iterator(merkle_root.begin()), 32, merkle_root_to_compare.begin());
  ASSERT_EQ(parser.get_merkle_root(), merkle_root_to_compare);
}
