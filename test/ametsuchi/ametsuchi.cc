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
#include <transaction_generated.h>
#include <flatbuffers/flatbuffers.h>
#include <gtest/gtest.h>
#include <spdlog/spdlog.h>
#include "../generator/tx_generator.h"

class Ametsuchi_Test : public ::testing::Test {
 protected:
  virtual void TearDown() { system(("rm -rf " + folder).c_str()); }

  std::string folder = "/tmp/ametsuchi/";
  ametsuchi::Ametsuchi ametsuchi_;

  Ametsuchi_Test() : ametsuchi_(folder) {}
};

TEST_F(Ametsuchi_Test, AssetTest) {
    // ASSERT_NO_THROW({
  flatbuffers::FlatBufferBuilder fbb(2048);

  // Create asset dollar
  auto blob = generator::random_transaction(
      fbb, iroha::Command::AssetCreate,
      generator::random_AssetCreate(fbb, "Dollar", "USA", "l1").Union());
  ametsuchi_.append(&blob);

  // Create account with id 1
  blob = generator::random_transaction(
      fbb, iroha::Command::AccountAdd,
      generator::random_AccountAdd(fbb, generator::random_account("1")).Union()
  );
  ametsuchi_.append(&blob);

  // Create account with id 2
  blob = generator::random_transaction(
      fbb, iroha::Command::AccountAdd,
      generator::random_AccountAdd(fbb, generator::random_account("2")).Union()
  );
  ametsuchi_.append(&blob);

  // Add currency to account 1
  blob = generator::random_transaction(
      fbb, iroha::Command::AssetAdd,
      generator::random_AssetAdd(
          fbb, "1",
          generator::random_asset_wrapper_currency(200,
                                                   2,
                                                   "Dollar",
                                                   "USA",
                                                   "l1")).Union()
  );
  ametsuchi_.append(&blob);

  {
    flatbuffers::FlatBufferBuilder fbb2(2048);
    auto reference_tx = flatbuffers::GetRoot<iroha::Transaction>(generator::random_transaction(
        fbb2, iroha::Command::AssetTransfer,
        generator::random_AssetTransfer(
            fbb2,
            generator::random_asset_wrapper_currency(100,
                                                     2,
                                                     "Dollar",
                                                     "USA",
                                                     "l1"),
            "1", "2").Union()
    ).data())->command_as_AssetTransfer();
    auto reference_1 = reference_tx->sender();
    auto reference_ln = reference_tx->asset_nested_root()->asset_as_Currency()->ledger_name();
    auto reference_dn = reference_tx->asset_nested_root()->asset_as_Currency()->domain_name();
    auto reference_cn = reference_tx->asset_nested_root()->asset_as_Currency()->currency_name();
    auto cur =
        flatbuffers::GetRoot<iroha::Asset>(ametsuchi_.accountGetAsset(
            reference_1,
            reference_ln,
            reference_dn,
            reference_cn,
            true).data)->asset_as_Currency();

    ASSERT_EQ(cur->amount(), 200);
  }

  // Transfer from 1 to 2
  blob = generator::random_transaction(
      fbb, iroha::Command::AssetTransfer,
      generator::random_AssetTransfer(
          fbb,
          generator::random_asset_wrapper_currency(100,
                                                   2,
                                                   "Dollar",
                                                   "USA",
                                                   "l1"),
          "1", "2").Union()
  );
  ametsuchi_.append(&blob);

  {
    flatbuffers::FlatBufferBuilder fbb2(2048);
    auto reference_tx = flatbuffers::GetRoot<iroha::Transaction>(generator::random_transaction(
        fbb2, iroha::Command::AssetTransfer,
        generator::random_AssetTransfer(
            fbb2,
            generator::random_asset_wrapper_currency(100,
                                                     2,
                                                     "Dollar",
                                                     "USA",
                                                     "l1"),
            "1", "2").Union()
    ).data())->command_as_AssetTransfer();
    auto reference_1 = reference_tx->sender();
    auto reference_ln = reference_tx->asset_nested_root()->asset_as_Currency()->ledger_name();
    auto reference_dn = reference_tx->asset_nested_root()->asset_as_Currency()->domain_name();
    auto reference_cn = reference_tx->asset_nested_root()->asset_as_Currency()->currency_name();
    auto cur =
        flatbuffers::GetRoot<iroha::Asset>(ametsuchi_.accountGetAsset(
            reference_1,
            reference_ln,
            reference_dn,
            reference_cn,
            true).data)->asset_as_Currency();

    ASSERT_EQ(cur->amount(), 100);
  }

  {
    flatbuffers::FlatBufferBuilder fbb2(2048);
    auto reference_tx = flatbuffers::GetRoot<iroha::Transaction>(generator::random_transaction(
        fbb2, iroha::Command::AssetTransfer,
        generator::random_AssetTransfer(
            fbb2,
            generator::random_asset_wrapper_currency(100,
                                                     2,
                                                     "Dollar",
                                                     "USA",
                                                     "l1"),
            "1", "2").Union()
    ).data())->command_as_AssetTransfer();
    auto reference_2 = reference_tx->receiver();
    auto reference_ln = reference_tx->asset_nested_root()->asset_as_Currency()->ledger_name();
    auto reference_dn = reference_tx->asset_nested_root()->asset_as_Currency()->domain_name();
    auto reference_cn = reference_tx->asset_nested_root()->asset_as_Currency()->currency_name();
    auto cur =
        flatbuffers::GetRoot<iroha::Asset>(ametsuchi_.accountGetAsset(
            reference_2,
            reference_ln,
            reference_dn,
            reference_cn,
            true).data)->asset_as_Currency();

    ASSERT_EQ(cur->amount(), 100);
  }

  ametsuchi_.commit();

  {
    flatbuffers::FlatBufferBuilder fbb2(2048);
    auto reference_tx = flatbuffers::GetRoot<iroha::Transaction>(generator::random_transaction(
        fbb2, iroha::Command::AssetTransfer,
        generator::random_AssetTransfer(
            fbb2,
            generator::random_asset_wrapper_currency(100,
                                                     2,
                                                     "Dollar",
                                                     "USA",
                                                     "l1"),
            "1", "2").Union()
    ).data())->command_as_AssetTransfer();
    auto reference_1 = reference_tx->sender();
    auto reference_ln = reference_tx->asset_nested_root()->asset_as_Currency()->ledger_name();
    auto reference_dn = reference_tx->asset_nested_root()->asset_as_Currency()->domain_name();
    auto reference_cn = reference_tx->asset_nested_root()->asset_as_Currency()->currency_name();
    auto cur =
        flatbuffers::GetRoot<iroha::Asset>(ametsuchi_.accountGetAsset(
            reference_1,
            reference_ln,
            reference_dn,
            reference_cn).data)->asset_as_Currency();

    ASSERT_EQ(cur->amount(), 100);
  }

  {
    flatbuffers::FlatBufferBuilder fbb2(2048);
    auto reference_tx = flatbuffers::GetRoot<iroha::Transaction>(generator::random_transaction(
        fbb2, iroha::Command::AssetTransfer,
        generator::random_AssetTransfer(
            fbb2,
            generator::random_asset_wrapper_currency(100,
                                                     2,
                                                     "Dollar",
                                                     "USA",
                                                     "l1"),
            "1", "2").Union()
    ).data())->command_as_AssetTransfer();
    auto reference_2 = reference_tx->receiver();
    auto reference_ln = reference_tx->asset_nested_root()->asset_as_Currency()->ledger_name();
    auto reference_dn = reference_tx->asset_nested_root()->asset_as_Currency()->domain_name();
    auto reference_cn = reference_tx->asset_nested_root()->asset_as_Currency()->currency_name();
    auto cur =
        flatbuffers::GetRoot<iroha::Asset>(ametsuchi_.accountGetAsset(
            reference_2,
            reference_ln,
            reference_dn,
            reference_cn).data)->asset_as_Currency();

    ASSERT_EQ(cur->amount(), 100);
  }
  //});
}
