/**
 * Copyright Soramitsu Co., Ltd. 2017 All Rights Reserved.
 * http://soramitsu.co.jp
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <block_parser_flatbuffers.h>
#include <gtest/gtest.h>

TEST(BLOCK_PARSER_FLATBUFFERS_TEST, PARSE) {
  flatbuffers::FlatBufferBuilder builder(1024);

  std::vector<flatbuffers::Offset<iroha::Transaction>> tx_vector;
  auto txs = builder.CreateVector(tx_vector);

  std::vector<flatbuffers::Offset<iroha::Signature>> sig_vector;
  auto peer_sigs = builder.CreateVector(sig_vector);

  std::vector<uint8_t> prev_hash_vector(32, 0x1);
  auto prev_hash = builder.CreateVector(prev_hash_vector);

  std::vector<uint8_t> merkle_root_vector(32, 0x0);
  auto merkle_root = builder.CreateVector(merkle_root_vector);

  std::vector<uint8_t> created_vector(25, 0x1);
  auto created = builder.CreateVector(created_vector);

  iroha::BlockBuilder blockBuilder(builder);

  blockBuilder.add_txs(txs);
  blockBuilder.add_peer_sigs(peer_sigs);
  blockBuilder.add_prev_hash(prev_hash);
  blockBuilder.add_length(250);
  blockBuilder.add_merkle_root(merkle_root);
  blockBuilder.add_height(500);
  blockBuilder.add_created(created);

  auto block = blockBuilder.Finish();
  builder.Finish(block);

  std::vector<uint8_t> blob{builder.GetBufferPointer(),
                            builder.GetBufferPointer() + builder.GetSize()};
  utils::BlockParserFlatbuffers parser(blob);

  ASSERT_EQ(parser.get_length(), 250);
  ASSERT_EQ(parser.get_height(), 500);
  std::array<uint8_t, 32> hash_to_compare;
  std::copy_n(std::make_move_iterator(prev_hash_vector.begin()), 32,
              hash_to_compare.begin());
  ASSERT_EQ(parser.get_prev_hash(), hash_to_compare);

  std::array<uint8_t, 32> root_to_compare;
  std::copy_n(std::make_move_iterator(merkle_root_vector.begin()), 32,
              root_to_compare.begin());
  ASSERT_EQ(parser.get_merkle_root(), root_to_compare);

  std::array<uint8_t, 25> created_to_compare;
  std::copy_n(std::make_move_iterator(created_vector.begin()), 25,
              created_to_compare.begin());
  ASSERT_EQ(parser.get_created(), created_to_compare);
}