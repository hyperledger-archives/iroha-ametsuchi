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

#ifndef AMETSUCHI_MERKLE_TREE_H
#define AMETSUCHI_MERKLE_TREE_H

#include <array>
#include <cstdint>
#include <list>
#include <vector>

extern "C" {
#include <SimpleFIPS202.h>
}


namespace ametsuchi {
namespace merkle {

const size_t HASH_LEN = 32;
using hash_t = std::array<uint8_t, HASH_LEN>;

/**
 * Minimalistic but very fast implementation of Merkle tree which uses array for
 * tree
 * - total number of hashes stored: size=2*leafs-1
 * - size * HASH_LEN bytes memory total
 * - get root in O(1)
 * - push in O(log2( size ))
 * - very convenient to store in file and mmap it, since its size does not
 * change
 * - number of leafs should be a power of 2
 * - if a number of leafs is not power of 2, it will ceil it to power of 2
 */
class MerkleTree {
 public:
  /**
   * Constructor
   * @param leafs - a number of leaf nodes in a tree.
   * @param max_rollback - maximum steps on which you can rollback a tree.
   * Default = \p leafs.
   */
  explicit MerkleTree(size_t leafs, size_t blocks = 1);

  /**
   * Get Merkle root. O(1)
   */
  hash_t root();

  /**
   * Push item to the tree and recalculate all hashes. O(log2(size)).
   * @param item
   */
  void push(const hash_t &item);
  void push(hash_t &&item);

  void rollback(size_t steps);
  size_t max_rollback();

  static hash_t hash(const hash_t &a, const hash_t &b);
  static hash_t hash(const std::vector<uint8_t> &data);
  static hash_t hash(const uint8_t* data, size_t size);

  /**
   * for debug only
   */
  void dump(size_t amount = 2);
 private:
  std::string printelement(std::vector<hash_t>& tree, size_t i, size_t amount);

  using tree_t = std::vector<hash_t>;
  std::list<tree_t> trees_;

  size_t max_blocks_;
  size_t size_;       // total size of a tree (rightmost leaf index)
  size_t leafs_;      // leafs, total. Power of 2
  size_t i_current_;  // a pointer to the next free cell in leafs
  size_t i_root_;     // a pointer to the merkle root

  inline size_t left(size_t parent);
  inline size_t right(size_t parent);
  inline size_t parent(size_t node);
};

}  // namespace merkle
}  // namespace ametsuchi

#endif  // AMETSUCHI_MERKLE_TREE_H
