/**
 * Copyright Soramitsu Co., Ltd. 2016 All Rights Reserved.
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
#include <ametsuchi/generated/commands_generated.h>
#include <ametsuchi/generated/key_generated.h>
#include <ametsuchi/generated/transaction_generated.h>
#include <ametsuchi/status.h>
#include <ametsuchi/tx_store/array.h>
#include <flatbuffers/flatbuffers.h>
#include <gtest/gtest.h>

namespace ametsuchi {
namespace tx_store {


class TX_New : public ::testing::Test {
 protected:
  TX_New() : a("/tmp/ametsuchi") {}

  virtual void TearDown() {
    remove("/tmp/ametsuchi/ledger");
    remove("/tmp/ametsuchi/ledger_index");
  }

  Ametsuchi a;
};


inline auto create_tx(std::string pubkey, std::string an, std::string dn,
                      std::string ln, std::string desc,
                      std::vector<uint8_t> hash) {
  using namespace flatbuffers;
  using namespace iroha;

  FlatBufferBuilder fbb;

  auto pubKey =
      CreatePublicKey(fbb, KeyAlgorithm::ed25519,
                      fbb.CreateVector({pubkey.begin(), pubkey.end()}));

  auto cmd = CreateCmdCreateAsset(
      fbb, fbb.CreateString(an.c_str()), fbb.CreateString(dn.c_str()),
      fbb.CreateString(ln.c_str()), fbb.CreateString(desc.c_str()));

  std::vector<uint8_t> s1{'0', '1', '2'};
  auto sig1 = CreateSignature(fbb, pubKey, fbb.CreateVector(s1), 1232);
  auto sig2 = CreateSignature(fbb, pubKey, fbb.CreateVector(s1), 1233);

  std::vector<Offset<Signature>> sigs{sig1, sig2};
  auto signatures = fbb.CreateVector(sigs);

  auto h = fbb.CreateVector(hash);

  auto tx = CreateTransaction(fbb, pubKey, Command::CmdCreateAsset, cmd.Union(),
                              signatures, h);

  fbb.Finish(tx);

  uint8_t* ptr = fbb.GetBufferPointer();

  return ByteArray{ptr, ptr + fbb.GetSize()};
}

TEST_F(TX_New, CreateAppendRead) {
  size_t size = 0;
  for (int i = 0; i < 100; i++) {
    auto tx = create_tx("public" + i, "an" + i, "dn1", "ln1", "desc1",
                        {1, 2, 3, 4, 5});

    size += 8 + tx.size();

    a.append(tx);
  }

  a.commit();

}


}  // namespace tx_store
}  // namespace ametsuchi
