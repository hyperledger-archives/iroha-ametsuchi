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
/*
#include <ametsuchi/ametsuchi.h>
#include <ametsuchi/generated/transaction_generated.h>
#include <flatbuffers/flatbuffers.h>
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

  size_t hash_len = 32;
  std::vector<uint8_t> hash_data(hash_len, 0x1);
  auto hash_field = builder.CreateVector(hash_data);
  auto transaction = iroha::CreateTransaction(
      builder, creator_pub_key, iroha::Command::AssetCreate,
      asset_create.Union(), signatures, hash_field);

  builder.Finish(transaction);

  uint8_t *buf = builder.GetBufferPointer();
  int size = builder.GetSize();

  //  auto transaction_object = flatbuffers::GetRoot<iroha::Transaction>(buf);
  std::vector<uint8_t> transaction_vector{buf, buf + size};
  //  flatbuffers::Vector<uint8_t>* v = (flatbuffers::Vector<uint8_t>*)
  //  transaction_vector.data();
  ametsuchi_.append(&transaction_vector);
  ametsuchi_.commit();
    builder.Clear();


  // now do asset add
  /*
  auto another_asset = iroha::CreateComplexAssetDirect(builder, "Dollar", "USA",
  "ledger1","NONE");
  builder.Finish(another_asset);
   */


  auto currency_to_add = iroha::CreateCurrencyDirect(
      builder, "Dollar", "USA", "ledger1", "description", 100, 2);
  builder.Finish(currency_to_add);


  //  auto currency_to_add_object =
  //  flatbuffers::GetRoot<iroha::Currency>(builder.GetBufferPointer());
  //  std::cout << "amount = " << currency_to_add_object->amount() << std::endl;
  std::vector<uint8_t> currency_vector{
      builder.GetBufferPointer(),
      builder.GetBufferPointer() + builder.GetSize()};
  builder.Clear();

  //  auto asset_add_offset = iroha::CreateAssetAdd(
  //      builder, builder.CreateString("1"),
  //      //      builder.CreateVector(builder.GetBufferPointer(),
  //      //      builder.GetSize()));
  //      builder.CreateVector(currency_vector.data(), currency_vector.size()));
  auto asset_add_offset =
      iroha::CreateAssetAddDirect(builder, "1", &currency_vector);
  //  builder.Finish(asset_add_offset);

  //  auto transaction2_offset = iroha::CreateTransaction(
  //      builder, creator_pub_key, iroha::Command::AssetAdd,
  //      asset_add_offset.Union(), signatures);
  auto transaction2_offset = iroha::CreateTransactionDirect(
      builder, "1", iroha::Command::AssetAdd, asset_add_offset.Union(),
      &signatures_vector, &hash_data);


  builder.Finish(transaction2_offset);

  auto transaction2 =
      flatbuffers::GetRoot<iroha::Transaction>(builder.GetBufferPointer());
  /*
  transaction2->command()
  auto asset_add = transaction2->command_as_AssetAdd();
  //  ASSERT_EQ(asset_add->asset_nested_root()->asset_type(),
  //  iroha::AnyAsset::Currency);
  auto asset_vector = asset_add->asset();
  auto asset = flatbuffers::GetRoot<iroha::Asset>(asset_vector->Data());
  ASSERT_EQ(asset->asset_type(), iroha::AnyAsset::Currency);
  //  auto a = asset->asset_type();
  auto currency = flatbuffers::GetRoot<iroha::Currency>(
      transaction2->command_as_AssetAdd()->asset()->data());
  std::cout << (currency->amount()) << std::endl;
  //  std::cout << "type = " <<
  //  transaction2->command_as_AssetAdd()->asset_nested_root()->asset_type()

  */
  buf = builder.GetBufferPointer();
  size = builder.GetSize();

  std::vector<uint8_t> transaction_vector2{buf, buf + size};
  //  flatbuffers::Vector<uint8_t>* v = (flatbuffers::Vector<uint8_t>*)
  //  transaction_vector.data();
  ametsuchi_.append(&transaction_vector2);
  ametsuchi_.commit();
  builder.Clear();

}
*/