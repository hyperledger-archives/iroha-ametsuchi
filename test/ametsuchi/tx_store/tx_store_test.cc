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

#include <gtest/gtest.h>
#include <ametsuchi/tx_store/array.h>
#include <ametsuchi/tx_store/index.h>

namespace ametsuchi {
namespace tx_store {

class TXStoreTest : public ::testing::Test {
 protected:
  TXStoreTest() : array_(array_path) {}

  virtual void TearDown() {
    remove(array_path.c_str());
    remove(index_path.c_str());
  }

  const std::string array_path = "/tmp/array",
    index_path = array_path+"_index";

  Array array_;

};

TEST_F(TXStoreTest, ArrayIndex) {
  ByteArray a1 = {1, 2, 3}, a2 = {2, 3, 4};
  array_.append(a1);
  array_.append(a2);
  array_.commit();


  ByteArray at1 = array_.get(0),
    at2 = array_.get(1);

  ASSERT_EQ(a1, at1);
  ASSERT_EQ(a2, at2);
}

}  // namespace tx_store
}  // namespace ametsuchi
