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

#include <gtest/gtest.h>
#include <ametsuchi/block_store/block_store_nudb.h>
#include <ametsuchi/utils/hash.h>

namespace ametsuchi{
namespace block_store {

TEST(block_store_nudb_test, sample_test) {
  BlockStoreNuDB bs("/tmp");
  auto str = std::string{0, 1, 2, 3};
  merkle_tree::hash_t str_hash;
  utils::sha3_256(str_hash.data(), reinterpret_cast<unsigned char*>(str.data()), str.size());
  bs.append(str_hash, str);
  ASSERT_EQ(str_hash, str);
}

}
}
