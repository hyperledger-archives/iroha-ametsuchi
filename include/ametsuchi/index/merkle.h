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

#include <ametsuchi/buffer.h>
#include <ametsuchi/exception.h>
#include <ametsuchi/globals.h>
#include <algorithm>
#include <cmath>
#include <vector>
#include <bitset>

namespace ametsuchi {
namespace index {
namespace merkle {

template <typename T>
constexpr size_t popcount(T n) {
    //std::bitset<sizeof(T)> b(n);
    return std::bitset<sizeof(T)>(n).count();
}

/**
 *   \brief Optimized merkle tree for append-only
 *   storage. With quick rollback property
 *
 *   As far as TXs in ledger cannot be undone
 *   except earlier than checkpoint thus we
 *   don't need to store entire tree and should
 *   only service capacity equals TXs to checkpoint
 *
 *                             _______[+]_________
 *                            /                   \
 *                           /                     \
 *                  ______[*]______                 [+]
 *                 /               \                 |
 *                /                 \                |
 *               /                   \               |
 *          __[-]__                 __[*]__         [+]
 *         /       \               /       \         |
 *      [^]         [^]         [-]         [*]     [+]
 *     /   \       /   \       /   \       /   \     |
 *   [^]   [^]   [^]   [^]   [^]   [^]   [-]   [*]  [t]
 *
 *   Current implementation stores `capacity`
 *   hash-paths marked as "*" and "-". In this example
 *   capacity equals 2. So rest blocks (maked as "^")
 *   doesn't stored and not wasting space.
 *
 *   After adding new hash of tx (marked as "t") merkle
 *   tree adds new hash-line marked as "+" and "t".
 *   Also it removes blocks marked with "-".
 *
 *   Tree stores $\sum^{capacity}_{n=0} (tx - n - 1)$
 *   elements where tx represents number tx added to
 *   the tree. Note that tree with capacity=0 still
 *   store $(tx - n - 1)$ hashes
 *  @tparam T type of the storing hash
 */
template <typename T>
class NarrowMerkleTree {
 public:
  // HashFn(t, T()) should be equal t
  using HashFn = std::function<T(const T &, const T &)>;
  // TODO: use linear buffer layout
  using Storage = std::vector<buffer::CircularStack<T>>;
  NarrowMerkleTree(HashFn, size_t capacity = 0);

  /**
   * Perform appending to the tree, recalculating
   * root and dropping outdated hashes
   */
  void add(T);

  /**
   * Drops provided num of TX's hashes and
   * floating parent blocks
   */
  void drop(size_t);

  /**
   * Calculate height of tree for `node`
   */
  static inline size_t height(size_t n) {
    size_t num = popcount(n) > 1 ? 1 : 0;
    while (n > 0) {
      n >>= 1;
      num++;
    }
    return num;
  }

  /**
   * Comparing common nodes on the path except
   * root between tx hash and subsequent tx hash
   */
  static inline size_t path_diff(size_t node) {
    // calculate difference of path from root
    // between `node` and `node + 1`
    auto x = node ^ (node + 1);
    // count number of leading zeros in x
    // in tree with height `node + 1`
    return floor(log2(node + 1)) - floor(log2(x));
  }

  inline size_t capacity() { return data[0].capacity(); }

  /**
   * Enforce copying because vector can be changed
   * Doesn't perform emptiness checks
   */
  inline T get_root() const;

  inline size_t size() const;

 private:
  Storage data;
  /**
   * Represents maximum number of tx hashes
   * that can be reseted w/o recalculation
   * fxd: moved to data[0].capacity()
   */
  // size_t capacity;
  /**
   * TXs' hashes added so far
   */
  size_t txs;
  /**
   * Function for parent vertices calculation
   */
  HashFn hash;

  /**
   * Extends data by the default capacity
   */
  void grow() { data.emplace_back(capacity()); }

  /**
   * Extends data by capacity + 2. So it's able to
   * save one more element for rollback and one more
   * for internal usage (see `add`)
   */
  void grow(size_t capacity) { data.emplace_back(capacity + 2); }
};


template <typename T>
NarrowMerkleTree<T>::NarrowMerkleTree(HashFn c, size_t capacity)
    : txs(0), hash(c) {
  if (capacity != 0) throw "Rollbacking is not supported yet";
  grow(capacity);
}

// TODO: test handling capacity() > 0
template <typename T>
void NarrowMerkleTree<T>::add(T t) {
  txs++;
  data[0].push(t);
  bool new_root = false;
  if (txs != 1 && height(txs) > height(txs - 1)) {
    grow();
    new_root = true;
  }
  
  // layer_idx is:
  //        0
  //    0       1
  //  0   1   2
  // 0 1 2 3 4 5 
  size_t layer_idx = txs - 1;
  bool need_expand = layer_idx % 2 == 0;
  // This loop traverse up to the tree
  for (auto prev = data.begin(), cur = prev + 1; cur != data.end();
       ++prev, ++cur, layer_idx /= 2) {
    // 0-th elems are actually outdated and can be removed
    // without any violation, that's why in grow capacity+2
    // but in that case easily to write, prob should be changed
    //
    // Traverse from left to right at the tree. prev means child
    // node, and cur its parent
    for (auto p = prev->begin(), c = cur->begin(); p != prev->end();
         p += 2, ++c) {
      bool is_lchild = layer_idx % 2 == 0;
      if (!is_lchild) need_expand = false;
      bool is_root = cur == data.end() - 1;
      T t1 = p[0];
      T t2 = is_lchild ? T() : p[1];
      if ((is_root && new_root) || need_expand) {
        cur->push(hash(t1, t2));
      } else {
        // there's probably an issue in index for capacity() > 0
        cur->begin()[(capacity() - layer_idx/2) % capacity()] = std::move(hash(t1, t2));
      }
    }
  }
}

// TODO: recount drop number
// now doesn't work if calling w/o filled data
template <typename T>
void NarrowMerkleTree<T>::drop(size_t num) {
  auto &t = txs;
  std::for_each(data.begin(), data.end(), [num, &t](auto &i) {
    auto n = std::min(i.size() - 1, num);
    t -= n;
    i.pop(n);
  });
}

template <typename T>
T NarrowMerkleTree<T>::get_root() const {
  const buffer::CircularStack<T> &cs = data.back();
  if (cs.size() == 0) throw Exception("Root is empty");
  return cs[0];
}

template <typename T>
size_t NarrowMerkleTree<T>::size() const {
  return txs;
}

}  // namespace merkle
}  // namespace index
}  // namespace ametsuchi
