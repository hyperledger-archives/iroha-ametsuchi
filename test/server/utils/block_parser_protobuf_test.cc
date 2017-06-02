#include <block.pb.h>
#include <block_parser_protobuf.h>
#include <gtest/gtest.h>
#include <fstream>
#include <action.pb.h>

using utils::BlockParserProtobuf;

TEST(BLOCK_PARSE_PROTOBUF_TEST, PARSE_TEST) {
  iroha::Block block;
  auto transaction = block.add_txs();
  transaction->set_created(125);
  transaction->set_nonce(25);
  auto action = transaction->add_actions();

  iroha::AddAccount* add_account = action->mutable_add_account();
  add_account->set_account_id(1);
  add_account->set_name("account_name");

  auto action_sign = transaction->add_sigs();
  action_sign->set_sig("action_sig");
  action_sign->set_pubkey("sign_pubkey");

  auto signature = block.add_peer_sigs();
  signature->set_pubkey("asd");
  signature->set_sig("123");

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
//  std::string res;
//  block.SerializeToString(&res);
//  std::cout << res << std::endl;

  BlockParserProtobuf parser(filename);
  ASSERT_EQ(parser.get_length(), 250);
  ASSERT_EQ(parser.get_height(), 500);
  ASSERT_EQ(parser.get_created(), 1);

  std::array<uint8_t, 32> hash_to_compare;
  std::copy_n(std::make_move_iterator(hash.begin()), 32,
              hash_to_compare.begin());
  ASSERT_EQ(parser.get_prev_hash(), hash_to_compare);

  std::array<uint8_t, 32> merkle_root_to_compare;
  std::copy_n(std::make_move_iterator(merkle_root.begin()), 32,
              merkle_root_to_compare.begin());
  ASSERT_EQ(parser.get_merkle_root(), merkle_root_to_compare);

  auto txs = block.txs();
  auto tx = txs[0];
  auto actions = tx.actions();
  auto a_action = actions[0];
  auto a_add_account = a_action.add_account();
  ASSERT_EQ(a_add_account.account_id(), 1);
  ASSERT_EQ(a_add_account.name(), "account_name");

  auto tx_sig = tx.sigs()[0];
  ASSERT_EQ(tx_sig.sig(), "action_sig");
  ASSERT_EQ(tx_sig.pubkey(), "sign_pubkey");
}
