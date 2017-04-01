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

#include <ametsuchi/generated/commands_generated.h>
#include <ametsuchi/generated/key_generated.h>
#include <ametsuchi/generated/transaction_generated.h>
#include <ametsuchi/status.h>
#include <ametsuchi/tx_store/index.h>
#include <ametsuchi/tx_store/tx_store.h>
#include <flatbuffers/flatbuffers.h>
#include <gtest/gtest.h>

namespace ametsuchi {
namespace tx_store {

inline auto create_tx(std::string pubkey, std::string an, std::string dn,
                      std::string ln, std::string desc,
                      std::vector<uint8_t> hash) {
  using namespace flatbuffers;
  using namespace iroha::api;
  using vec = std::vector;

  FlatBufferBuilder fbb;

  auto pubKey = CreatePublicKey(fbb, KeyAlgorithm::ed25519,
                                fbb.CreateVector(vec{pubkey.c_str()}));

  auto cmd = CreateCreateAsset(
      fbb, fbb.CreateString(an.c_str()), fbb.CreateString(dn.c_str()),
      fbb.CreateString(ln.c_str()), fbb.CreateString(desc.c_str()));

  auto sig1 = CreateSignature(fbb, pubKey, fbb.CreateVector(vec{"123"}), 1232);
  auto sig2 = CreateSignature(fbb, pubKey, fbb.CreateVector(vec{"234"}), 1233);
  auto sig3 = CreateSignature(fbb, pubKey, fbb.CreateVector(vec{"345"}), 1234);

  auto signatures = fbb.CreateVector(std::vector{sig1, sig2, sig3});

  auto h = fbb.CreateVector(hash);

  auto tx = CreateTransaction(fbb, pubKey, Command::CreateAsset, cmd.Union(),
                              signatures, h);

  fbb.Finish(tx);

  uint8_t* ptr = fbb.GetBufferPointer();

  return ByteArray{ptr, ptr + fbb.GetSize()};
}

TEST(TX_Store, CreateTxAndAppend) {
  for (int i = 0; i < 1; i++) {
    auto tx = create_tx("public" + i, "an" + i, "dn1", "ln1", "desc1",
                        {1, 2, 3, 4, 5});

    tx_store::append(tx);
  }
  tx_store::commit();

  auto tx = tx_store::getAddTxByCreator("public1");

  printf("tx: %s\n", dump(tx));
}

}  // namespace tx_store
}  // namespace ametsuchi
