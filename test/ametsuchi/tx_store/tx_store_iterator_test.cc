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
#include <ametsuchi/tx_store/iterator.h>


namespace ametsuchi {
namespace tx_store {

class TXStoreIteratorTest : public ::testing::Test {
 protected:
  TXStoreIteratorTest() : array_(array_path), index_(index_path) {}

  virtual void TearDown() {
    remove(array_path.c_str());
    remove(index_path.c_str());
  }

  const std::string array_path = "/tmp/array",
      index_path = "/tmp/array_index";

  Array array_;
  Index index_;
};

TEST_F(TXStoreIteratorTest, ArrayIterator) {
  const int N = 10000;
  ByteArray test_set[N];
  for (int i = 0; i < N; ++i) {
    for (int j = 0; j < 500; ++j) {
      test_set[i].push_back(1);
    }
    array_.append(test_set[i]);
  }


  int i = 0;
  auto end = array_.end();
  auto begin = array_.begin();
  for (auto it = begin;it < end; it++) {
    ASSERT_EQ(test_set[i], *it);
    i++;
  }

}

TEST_F(TXStoreIteratorTest, MultiGetTest) {
  const int N = 10000;
  ByteArray test_set[N];
  for (int i = 0; i < N; ++i) {
    for (int j = 0; j < 500; ++j) {
      test_set[i].push_back(1);
    }
    array_.append(test_set[i]);
  }

  for (int i = 0; i < N; ++i) {
    ASSERT_EQ(array_.get(i), test_set[i]);

  }

}


TEST_F(TXStoreIteratorTest, BackIterator) {
  const int N = 5;
  ByteArray test_set[N];
  for (int i = 0; i < N; ++i) {
    for (byte_t j = 0; j < i + 2; ++j) {
      test_set[i].push_back(j);
    }
    array_.append(test_set[i]);
  }

  int i = N-1;
  for (auto it = array_.end();it > array_.begin(); it--) {
    ASSERT_EQ(test_set[i], *it);
    i-=1;
  }

}

}  // namespace tx_store
}  // namespace ametsuchi
