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

#include <ametsuchi/globals.h>
#include <ametsuchi/table/fixed_table.h>
#include <ametsuchi/table/table.h>

#include "lib.h"

namespace ametsuchi {
namespace table {

template <typename T>
using Iter = typename FixedTable<T>::BidirIterator;

constexpr auto fb = FB(A, 0x12345678, 18345e-3);

TEST_F(FileTestHelper, IteratorIncrements) {
  FixedTable<ExampleFB> table(filename);
  table.appendBatch(std::vector<ExampleFB>(num, fb));
  {
    size_t count = 0;
    for (auto i = table.begin(); i < table.end(); ++i) {
      count++;
      ASSERT_FBEQ(*i, fb);
    }
    ASSERT_EQ(count, num);
  }
  {
    auto count = 0;
    for (auto i = table.begin(); i < table.end(); i++) {
      count++;
      ASSERT_FBEQ(*i, fb);
    }
    ASSERT_EQ(count, num);
  }
}

TEST_F(FileTestHelper, IteratorDecrement) {
  FixedTable<ExampleFB> table(filename);
  table.appendBatch(std::vector<ExampleFB>(num, fb));
  {
    size_t count = 0;
    auto i = table.end();
    for (--i; i > table.begin(); --i) {
      count++;
      ASSERT_FBEQ(*i, fb);
    }
    ASSERT_FBEQ(*i, fb);
    ASSERT_EQ(count + 1, num);
  }
  {
    size_t count = 0;
    auto i = table.end();
    for (i--; i > table.begin(); i--) {
      count++;
      ASSERT_FBEQ(*i, fb);
    }
    ASSERT_FBEQ(*i, fb);
    ASSERT_EQ(count + 1, num);
  }
}
}
}
