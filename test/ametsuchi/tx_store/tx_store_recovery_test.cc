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

namespace ametsuchi {
namespace tx_store {

class TXStoreRecoveryTest : public ::testing::Test {
protected:
  TXStoreRecoveryTest() {}

  virtual void TearDown() {
    remove(array_path.c_str());
    remove(index_path.c_str());
  }

  const std::string array_path = "/tmp/array", index_path = "/tmp/array_index";
};

TEST_F(TXStoreRecoveryTest, CommitRollbackTest) {
//  const std::string array_path = "/tmp/array";
  Array array_(array_path);
  ByteArray set1 = {0x1, 0x2};
  ByteArray set2 = {0x3, 0x4};
  ByteArray set3 = {0x5};

  array_.append(set1);
  ASSERT_EQ(array_.get(0), ByteArray{});
  array_.commit();
  ASSERT_EQ(array_.get(0), set1);

  array_.append(set2);
  ASSERT_ANY_THROW(array_.get(1));
  array_.append(set3);

  array_.rollback();

  array_.append(set2);
  array_.append(set3);
  array_.commit();
  ASSERT_EQ(array_.get(1), set2);
  ASSERT_EQ(array_.get(2), set3);

//  remove(array_path.c_str());
//  remove((array_path + "_index").c_str());
}

TEST_F(TXStoreRecoveryTest, BatchAppendTest) {
  const std::string array_path = "/tmp/array";
  Array array_(array_path);
  ByteArray set1 = {0x1, 0x2};
  ByteArray set2 = {0x3, 0x4};
  ByteArray set3 = {0x5};

  array_.batch_append(std::vector<ByteArray >{set1, set2, set3});

  ASSERT_NE(array_.get(0), set1);
  ASSERT_NE(array_.get(1), set2);

  array_.rollback();

  array_.batch_append(std::vector<ByteArray >{set2, set3});
  array_.commit();

  ASSERT_EQ(array_.get(0), set2);
  ASSERT_EQ(array_.get(1), set3);

//  remove(array_path.c_str());
//  remove((array_path + "_index").c_str());
}


TEST(TXStoreRecoveryTest1, CrashTestCreate) {
  const std::string array_path = "/tmp/array";
  Array array_(array_path);
  ByteArray set1 = {0x1, 0x2};
  ByteArray set2 = {0x3, 0x5};
  std::cout << std::endl << "appended to " << array_.append(set1) << std::endl;
  array_.commit();
  std::cout << std::endl << "appended to " << array_.append(set2) << std::endl;
  array_.get(0);
}

TEST(TXStoreRecoveryTest1, CrashTestRead) {
  const std::string array_path = "/tmp/array";
  Array array_(array_path);
  ByteArray set1 = {0x1, 0x2};
  ByteArray set2 = {};
  ByteArray res1 = array_.get(0);
  std::cout << std::endl << "appended to " << array_.append(set2) << std::endl;
  array_.commit();
  ByteArray res2 = array_.get(1);
  ASSERT_EQ(set1, res1);
  ASSERT_EQ(set2, res2);
}


TEST(TXStoreRecoveryTest1, RecoveryRead) {
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

}  // tx_store
}  // ametsuchi