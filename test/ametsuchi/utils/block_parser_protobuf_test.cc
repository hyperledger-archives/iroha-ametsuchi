#include <gtest/gtest.h>
#include <block.pb.h>
#include <ametsuchi/utils/block_parser_protobuf.h>

TEST(BLOCK_PARSE_PROTOBUF_TEST, PARSE_TEST){
  iroha::Block block;
//  auto transaction = block.add_txs();
//  auto signature = block.add_peer_sigs();

//  std::vector<char> hash(32, '0');
//  block.set_prev_hash(hash.data());
//
//  block.set_length(250);
//
//  std::vector<char> merkle_root(32, '1');
//  block.set_merkle_root(merkle_root.data());
//
//  block.set_height(500);
//  block.set_created(1);

//  fstream output("/tmp/temp", ios::out | ios::trunc | ios::binary);
//  block.SerializePartialToOstream(&output);
}