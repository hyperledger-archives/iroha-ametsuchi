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

#include <ametsuchi/comparator.h>
#include <ametsuchi/status.h>
#include <gtest/gtest.h>

namespace ametsuchi {

TEST(ComparatorTest, ByteArrayValues) {
  const size_t size = 50;

  ByteArray a(size);
  ByteArray b(size);

  uint32_t seed = 0;
  for (size_t i = 0; i < size; i++) {
    uint8_t num = (uint8_t)(rand_r(&seed) % 256);
    a[i] = num;
    b[i] = num;
  }

  comparator::ByteArrayCmp cmp;

  ASSERT_EQ(cmp(a, b), 0);

  a.push_back(0u);
  ASSERT_EQ(a.size() - 1, size);
  // a > b
  ASSERT_GT(cmp(a, b), 0);

  b.push_back(1u);
  ASSERT_EQ(b.size() - 1, size);
  // a < b
  ASSERT_LT(cmp(a, b), 0);
}

TEST(ComparatorTest, ByteArrayLengths) {
  comparator::ByteArrayCmp cmp;

  ByteArray a(20, 0u);
  ByteArray b(10, 1u);
  // a < b
  EXPECT_LT(cmp(a, b), 0);

  ByteArray c(10, 2u);
  ByteArray d(9, 3u);
  // c > d
  EXPECT_GT(cmp(c, d), 0);

  ByteArray e(10, 0u);
  ByteArray f(20, 0u);
  // e == f
  EXPECT_EQ(cmp(e, f), 0);

  ByteArray z;
  ByteArray x(10, 0u);
  // null = 0
  // z == x
  EXPECT_EQ(cmp(z,x), 0);
}
}
