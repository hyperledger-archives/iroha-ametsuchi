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

#include <ametsuchi/ametsuchi.h>
#include <ametsuchi/generated/transaction_generated.h>
#include <gtest/gtest.h>
#include <lmdb.h>
#include <spdlog/spdlog.h>

class Ametsuchi_Test : public ::testing::Test {
 protected:
  virtual void TearDown() { system(("rm -rf " + folder).c_str()); }

  std::string folder = "/tmp/ametsuchi/";
  ametsuchi::Ametsuchi ametsuchi_;

  Ametsuchi_Test() : ametsuchi_(folder) {}
};


TEST_F(Ametsuchi_Test, AssetTest) {
  // create command
  flatbuffers::FlatBufferBuilder builder;

  auto asset_name = builder.CreateString("Dollar");
  auto domain_name = builder.CreateString("USA");
  auto ledger_name = builder.CreateString("ledger1");
  auto creator_pub_key = builder.CreateString("1");
  auto asset_create = iroha::CreateAssetCreate(builder, asset_name, domain_name,
                                               ledger_name, creator_pub_key);
  // create signature
  uint8_t sign_data[] = {0x1, 0x2, 0x3};
  auto signature_field = builder.CreateVector(sign_data, 3);
  uint64_t timestamp = 123823;
  auto signature = iroha::CreateSignature(builder, creator_pub_key,
                                          signature_field, timestamp);

  // create transaction
  std::vector<flatbuffers::Offset<iroha::Signature>> signatures_vector;
  signatures_vector.push_back(signature);
  auto signatures = builder.CreateVector(signatures_vector.data(), 1);
  auto transaction = iroha::CreateTransaction(builder, creator_pub_key,
                                              iroha::Command::AssetCreate,
                                              asset_create.Union(), signatures);

  builder.Finish(transaction);

  uint8_t *buf = builder.GetBufferPointer();
  int size = builder.GetSize();

  //  auto transaction_object = flatbuffers::GetRoot<iroha::Transaction>(buf);
  std::vector<uint8_t> transaction_vector{buf, buf + size};
  ametsuchi_.append(transaction_vector);
  ametsuchi_.commit();
}
