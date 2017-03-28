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
#include <ametsuchi/tx_store/index.h>
#include <ametsuchi/tx_store/iterator.h>
#include <gtest/gtest.h>


namespace ametsuchi {
namespace tx_store {

class TXStoreIteratorTest : public ::testing::Test {
 protected:
  TXStoreIteratorTest() : array_(array_path) {}

  virtual void TearDown() {
    remove(array_path.c_str());
    remove(index_path.c_str());
  }

  const std::string array_path = "/tmp/array", index_path = "/tmp/array_index";

  Array array_;

};

TEST_F(TXStoreIteratorTest, ArrayIterator) {
  const int N = 2;
  std::vector<ByteArray> test_set(N, ByteArray(500, 1));
  array_.append_batch(test_set);
  array_.commit();

  int i = 0;
  auto end = array_.end();
  auto begin = array_.begin();
  for (auto it = begin; it < end; it++) {
    ASSERT_EQ(test_set[i], *it);
    i++;
  }

}



TEST_F(TXStoreIteratorTest, MultiGetTest) {
  const int N = 10000;
  std::vector<ByteArray> test_set(N, ByteArray(500, 1));
  array_.append_batch(test_set);
  array_.commit();

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
  array_.commit();

  int i = N - 1;
  for (auto it = array_.end()-1; it >= array_.begin(); it--) {
    ASSERT_EQ(test_set[i], *it);
    i -= 1;
  }
}




TEST_F(TXStoreIteratorTest, MultiTransactions) {
  const int N = 2;
  std::vector<ByteArray> test_set(N, ByteArray(500, 1));
  array_.append_batch(test_set);
  array_.commit();

  int i = 0;
  auto end = array_.end();
  auto begin = array_.begin();
  for (auto it = begin; it < end; it++) {
    ASSERT_EQ(test_set[i], *it);
    i++;
  }

  std::vector<ByteArray> test_set2(N, ByteArray(500, 2));
  array_.append_batch(test_set2);
  array_.commit();

  auto end2 = array_.end()-1;
  i = N - 1;
  for(auto it = end2;it < end2-2;it--)
  {
    ASSERT_EQ(test_set[i], *it);
    i--;
  }

}




}  // namespace tx_store
}  // namespace ametsuchi
