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

#pragma once

#include <ametsuchi/merkle_tree/narrow_merkle_tree.h.h>
#include <ametsuchi/exception.h>
#include <array>
#include <cstdint>
#include <vector>

extern "C" {
#include <SimpleFIPS202.h>
}

#ifndef MERKLE_CAPACITY_SIZE
#define MERKLE_CAPACITY_SIZE (8)  // the number of leafs in merkle tree
#endif

namespace ametsuchi {
namespace merkle {

const size_t HASH_LEN = 32;
using hash_t = std::array<uint8_t, HASH_LEN>;


class WrapperMerkleTree {
 public:
  /**
   * Constructor
   * @param block_size - a number of ametsuchi block size
   */
  explicit WrapperMerkleTree(size_t block_size);

  /**
   * Get Merkle root. in most case O(1). worst case O( log_capacity( node ) ).
   */
  hash_t root();

  /**
   * Push item to the tree and recalculate all hashes. O(log_capacity(size)).
   * @param item
   */
  void push(const hash_t &item);

  void push(hash_t &&item);

  const auto& data() const;

  /**
   * drop specify index.
   * @return number of blocks after erase hashes.
   */
  size_t drop(size_t);

  void rollback();

  static hash_t hash(const hash_t &a, const hash_t &b);

  static hash_t hash(const std::vector<uint8_t> &data);

 private:
  NarrowMerkleTree<hash_t> block_tree_;
  NarrowMerkleTree<hash_t> tx_tree_;
  block_size_;
};
}
}