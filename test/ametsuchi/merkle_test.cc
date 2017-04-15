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

#include <ametsuchi/merkle_tree/merkle_tree.h>
#include <ametsuchi/merkle_tree/narrow_merkle_tree.h>
#include <gtest/gtest.h>

namespace ametsuchi {
namespace merkle {

constexpr size_t size = 10;

TEST(Merkle, Creation) {
  NarrowMerkleTree<uint64_t> tree([](auto i, auto j) { return i + j; });
}

TEST(Merkle, Addition) {
  NarrowMerkleTree<size_t> tree([](auto i, auto j) { return i + j; });
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
  NarrowMerkleTree<size_t> tree([](auto i, auto j) { return i + j; });
  for (size_t i = 0; i < size; ++i) tree.add(i);
  tree.drop(6);
  /*
   * data[3]:(0+1 + 2+3  +  4+5 + 6+7)
   * data[2]:(0+1 + 2+3)   (4+5 + 6+7)
   * data[1]:                    (6+7) (8+9)
   * data[0]:                           8 9
   *   ||
   *   v
   * data[3]:
   * data[2]:(0+1 + 2+3)
   * data[1]:
   * data[0]:
   */
  ASSERT_EQ(tree.size(), 4);
  ASSERT_EQ(tree.get_root(), 3 * 4 / 2);

  for (size_t i = 4; i < size; i++) tree.add(i);
  ASSERT_EQ(tree.size(), size);
  ASSERT_EQ(tree.get_root(), 9 * 10 / 2);

  tree.drop(8);
  /*
   * data[3]:(0+1 + 2+3  +  4+5 + 6+7)
   * data[2]:(0+1 + 2+3)   (4+5 + 6+7)
   * data[1]:                    (6+7)
   * data[0]:
   */
  ASSERT_EQ(tree.size(), 8);
  ASSERT_EQ(tree.get_root(), 7 * 8 / 2);

  for (size_t i = 8; i < size; i++) tree.add(i);
  ASSERT_EQ(tree.size(), size);
  ASSERT_EQ(tree.get_root(), 9 * 10 / 2);

  tree.drop(0);
  ASSERT_EQ(tree.size(), 0);
}

TEST(Merkle, Hegiht) {
  {
    constexpr size_t heights[] = {
        0, 1, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5,
        5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
        6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6};
    constexpr auto elems = sizeof(heights) / sizeof(heights[0]);
    NarrowMerkleTree<uint64_t> tree([](auto i, auto j) { return i + j; }, 2);
    for (size_t i = 0; i < elems; ++i) {
      ASSERT_EQ(tree.height(i), heights[i]);
    }
  }
  {
    constexpr size_t heights[] = {0, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
                                  2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3};
    constexpr auto elems = sizeof(heights) / sizeof(heights[0]);
    NarrowMerkleTree<uint64_t> tree([](auto i, auto j) { return i + j; }, 4);
    for (size_t i = 0; i < elems; ++i) {
      ASSERT_EQ(tree.height(i), heights[i]);
    }
  }
}

TEST(Merkle, Statics) {
  {
    constexpr size_t diffs[] = {0, 0, 1, 0, 2, 1, 2, 0, 3,
                                2, 3, 1, 3, 2, 3, 0, 4};
    constexpr auto elems = sizeof(diffs) / sizeof(diffs[0]);
    for (size_t i = 0; i < elems; ++i) {
      ASSERT_EQ(NarrowMerkleTree<uint64_t>::path_diff(i), diffs[i]) << i;
    }
  }
}

TEST(Merkle, ExtendAddition) {
  NarrowMerkleTree<uint64_t> tree([](auto i, auto j) { return i + j; }, 8);
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

TEST(Merkle, ExtendDropping) {
  NarrowMerkleTree<uint64_t> tree([](auto i, auto j) { return i + j; }, 8);
  for (size_t i = 0; i < size; ++i) tree.add(i);
  tree.drop(6);
  ASSERT_EQ(tree.size(), 6);
  ASSERT_EQ(tree.get_root(), 5 * 6 / 2);

  for (size_t i = 6; i < size; i++) tree.add(i);
  ASSERT_EQ(tree.size(), size);
  ASSERT_EQ(tree.get_root(), 9 * 10 / 2);

  tree.drop(8);
  ASSERT_EQ(tree.size(), 8);
  ASSERT_EQ(tree.get_root(), 7 * 8 / 2);

  for (size_t i = 8; i < size; i++) tree.add(i);
  ASSERT_EQ(tree.size(), size);
  ASSERT_EQ(tree.get_root(), 9 * 10 / 2);

  tree.drop(0);
  ASSERT_EQ(tree.size(), 0);
}

auto str2hex = [](const std::string &hex) {
  const std::string alp = "0123456789abcdef";

  hash_t out;
  size_t ptr = 0;
  for (size_t i = 0; i < hex.length(); i += 2) {
    out[ptr++] = alp.find(hex[i]) * 16 + alp.find(hex[i + 1]);
  }

  return out;
};

// sha3_256 of "1", https://emn178.github.io/online-tools/sha3_256.html
hash_t h =
    str2hex("67b176705b46206614219f47a05aee7ae6a3edbe850bbbe214c536b989aea4d2");

// calculated with
// https://gist.github.com/Warchant/774ad06f7c3e0e17bad0359f4bae3411
// for tree with 4 leafs
std::vector<hash_t> roots = {
    str2hex("67b176705b46206614219f47a05aee7ae6a3edbe850bbbe214c536b989aea4d2"),
    str2hex("80141312b118fb9099ec9277809de617cacc05a33312d3802d70a3db0d57e44a"),
    str2hex("a789f4a3c9b124149f17fda28573de4f4a9f205d428339f6308324b0d61917ed"),
    str2hex("5c3924aaff4fac9a843bc2229fec34ec01843accb996faf949d3a51e0800c2f8"),
    str2hex("1408519e75164ccfce0bd858b1c32f53dda2422f77adc2166508cbbd658ebab4"),
    str2hex("eef6f63a47272f280cb39f72d2ada587411e95c488d402044df20072630a8b9f"),
    str2hex("e618578c471c0eae2a05121d8bbe4a67cccb53f9511286bb60d93c295afcd263"),
    str2hex(
        "f13ec23c48a494d98b10cd5144dd0e3d8e2619222921e72aa403a0cead44a296")};

TEST(NaiveMerkle, Tree4_1block) {
  // should with size 4
  merkle::MerkleTree tree(4);
  for (size_t i = 0; i < 8; i++) {
    tree.push(h);
    auto root = tree.root();
    ASSERT_EQ(root, roots[i]);
  }
}

TEST(NaiveMerkle, Tree4_2blocks) {
  // tree with size 4, which stores 2 blocks
  merkle::MerkleTree tree(4, 2);
  for (size_t i = 0; i < 8; i++) {
    tree.push(h);
    auto root = tree.root();
    ASSERT_EQ(root, roots[i]) << "Expected root is different.";
  }
}

TEST(NaiveMerkle, Tree4_1_Rollback) {
  size_t items_total = 3;

  merkle::MerkleTree tree(4);
  for (size_t i = 0; i < items_total; i++) {
    tree.push(h);
    auto root = tree.root();
    ASSERT_EQ(root, roots[i]) << "Expected root is different";
  }

  for (size_t steps = 1; steps < 3; steps++) {
    tree.rollback(1);
    auto root = tree.root();
    ASSERT_EQ(root, roots[items_total - steps]) << "Rollback on " << steps
                                                << " steps back failed";
  }
}


TEST(NaiveMerkle, Tree128_100k_pushes) {
  merkle::MerkleTree tree(128);
  for (size_t i = 0; i < 100000; i++) {
    tree.push(roots[i % 8]);
  }
  SUCCEED();
}

}  // namespace merkle
}  // namespace ametsuchi
