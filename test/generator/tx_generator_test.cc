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

#include "tx_generator.h"
#include <gtest/gtest.h>

TEST(Generator, RandomNumber) {
  for (size_t i = 0; i < 1000; i++) {
    auto n = generator::random_number(50, 100);
    ASSERT_LT(n, 100);
    ASSERT_GE(n, 50);
  }
}

TEST(Generator, RandomPrintableChar) {
  for (size_t i = 0; i < 1000; i++) {
    auto n = generator::random_printable_char();
    ASSERT_LT(n, 127);
    ASSERT_GE(n, 32);
  }
}

TEST(Generator, RandomPrintableString) {
  std::string s;
  ASSERT_NO_THROW(s = generator::random_string(10));
  ASSERT_EQ(s.size(), 10);
}

TEST(Generator, RandomBlob) {
  std::vector<uint8_t> v(100);
  ASSERT_NO_THROW(v = generator::random_blob(100));
  ASSERT_EQ(v.size(), 100);
}

TEST(Generator, RandomBase64String) {
  std::string s;
  ASSERT_NO_THROW(s = generator::random_base64_string(100));
  ASSERT_EQ(s.size(), 100);
}

TEST(Generator, RandomPublicKey) {
  std::string s;
  ASSERT_NO_THROW(s = generator::random_public_key());
  ASSERT_EQ(s.size(), generator::PUB_KEY_LENGTH_);
}

#define TEST_COMMAND(CMD)                                                  \
  TEST(Generator, RandomTx_##CMD) {                                        \
    ASSERT_NO_THROW({                                                      \
      flatbuffers::FlatBufferBuilder fbb(2048);                            \
      auto v = generator::random_transaction(                              \
          fbb, iroha::Command::CMD, generator::random_##CMD(fbb).Union()); \
      auto tx = flatbuffers::GetRoot<iroha::Transaction>(v.data());        \
    });                                                                    \
  }

TEST_COMMAND(AccountAdd)
TEST_COMMAND(AccountRemove)
TEST_COMMAND(PeerAdd)
TEST_COMMAND(PeerRemove)
TEST_COMMAND(AssetAdd)
TEST_COMMAND(AssetRemove)
TEST_COMMAND(AssetTransfer)
TEST_COMMAND(AssetCreate)