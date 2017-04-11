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

#include <ametsuchi/merkle_tree/wrapper_merkle_tree.h>

namespace ametsuchi {
namespace merkle {


WrapperMerkleTree::WrapperMerkleTree(size_t block_size) : block_size_(block_size), block_tree_(hash,MERKLE_CAPACITY_SIZE), tx_tree_(hash,MERKLE_CAPACITY_SIZE) {
}

hash_t WrapperMerkleTree::root() { return block_tree_.get_root(); }

void WrapperMerkleTree::push(const hash_t &item) {
  tx_tree_.add( item );
  if( tx_tree_.size() % block_size_ == 0 ) {
    block_size_.add( tx_tree_.get_root() );
  }
}

void WrapperMerkleTree::push(hash_t &&item) {
  auto it = std::move(item);
  this->push(it);
}

const auto& WrapperMerkleTree::data() const {
  return block_size_.data();
}

void WrapperMerkleTree::rollback() {
  tx_tree.drop(0);
}

hash_t WrapperMerkleTree::hash(const hash_t &a, const hash_t &b) {
  std::array<uint8_t, 2 * HASH_LEN> input;
  hash_t output;

  // concatenate input hashes
  std::copy(a.begin(), a.end(), &input[0]);
  std::copy(b.begin(), b.end(), &input[HASH_LEN]);

  SHA3_256(output.data(), input.data(), input.size());

  return output;
}

hash_t WrapperMerkleTree::hash(const std::vector<uint8_t> &data) {
  hash_t output;
  SHA3_256(output.data(), data.data(), data.size());
  return output;
}

}  // namespace merkle
}  // namespace ametsuchi
