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

#include <ametsuchi/tx_store/array.h>
#include <gtest/gtest.h>

namespace ametsuchi{
namespace tx_store{

TEST(TXStoreIteratorTest2, CrashTestCreate) {
  const std::string array_path = "/tmp/array";
  Array array_(array_path);
  ByteArray set1 = {0x1, 0x2};
  ByteArray set2 = {0x3, 0x5};
  array_.append(set1);
//  array_.crash_append(set2);
  array_.get(0);
}

TEST(TXStoreIteratorTest2, CrashTestRead) {
  const std::string array_path = "/tmp/array";
  Array array_(array_path);
  ByteArray set1 = {0x1, 0x2};
  ByteArray set2 = {};
  ByteArray res1 = array_.get(0);
  array_.append(set2);
  ByteArray res2 = array_.get(1);
  ASSERT_EQ(set1, res1);
  ASSERT_EQ(set2, res2);
}


TEST(TXStoreIteratorTest2, RecoveryRead) {
  const std::string array_path = "/tmp/array";
  Array array_(array_path);
  ByteArray set1 = {0x1, 0x2};
  ByteArray set2 = {};
  ByteArray res1 = array_.get(0);
  ByteArray res2 = array_.get(1);
  ASSERT_EQ(set1, res1);
  ASSERT_EQ(set2, res2);
  remove(array_path.c_str());
  remove((array_path + "_index").c_str());
}

} // tx_store
} // ametsuchi