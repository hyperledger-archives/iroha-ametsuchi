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

#include <ametsuchi/index/merkle.h>
#include <gtest/gtest.h>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <vector>
#include <iostream>

namespace ametsuchi {
namespace index {
namespace merkle {

constexpr auto size = 10;

TEST(Merkle, Creation) {
  NarrowMerkleTree<uint64_t> tree([](auto i, auto j) { return i + j; });
}

TEST(Merkle, Addition) {
  NarrowMerkleTree<uint64_t> tree([](auto i, auto j) { return i + j; });
  /*
   *                              55
   *              28                           27
   *       6              22              27
   *   1       5       9      13      17       10
   * 0   1   2   3   4   5   6   7   8   9   10
   */
  for (size_t i = 0, sum = 0; i < size * size; ++i, sum += i) {
    tree.add(i);
    ASSERT_EQ(tree.get_root(), sum);
  }
}

TEST(Merkle, Dropping) {
  NarrowMerkleTree<uint64_t> tree([](auto i, auto j) { return i + j; });
  for (auto i = 0; i < size; ++i) tree.add(i);
  tree.drop(2);
  ASSERT_LE(tree.size(), size);
  ASSERT_GE(tree.size(), 0);
}

TEST(Merkle, Statics) {
  {
    constexpr size_t heights[] = {
        0,
        1,
        2,
        3, 3,
        4, 4, 4, 4,
        5, 5, 5, 5, 5, 5, 5, 5,
        6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,};
    constexpr auto elems = sizeof(heights) / sizeof(heights[0]);
    for (size_t i = 0; i < elems; ++i) {
      ASSERT_EQ(NarrowMerkleTree<uint64_t>::height(i), heights[i]);
    }
  }
  {
    constexpr size_t diffs[] = {0, 0, 1, 0, 2, 1, 2, 0, 3, 2, 3, 1, 3, 2, 3, 0, 4};
    constexpr auto elems = sizeof(diffs) / sizeof(diffs[0]);
    for (size_t i = 0; i < elems; ++i) {
      ASSERT_EQ(NarrowMerkleTree<uint64_t>::path_diff(i), diffs[i]) << i;
    }
  }
}



}  // namespace merkle
}  // namespace index
}  // namespace ametsuchi
