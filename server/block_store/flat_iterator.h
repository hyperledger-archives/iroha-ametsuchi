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


#include "block_store.h"

namespace block_store {

class BlockStoreFlat::FlatIterator : public BlockStore::AbstractIterator {
 public:
  FlatIterator(BlockStoreFlat &bstore);
  FlatIterator(BlockStoreFlat &bstore, uint32_t offset);
  FlatIterator(const FlatIterator &it);
  // Operators
  bool operator==(const BlockStore::AbstractIterator &it) override;
  bool operator!=(const BlockStore::AbstractIterator &it) override;
  BlockStore::AbstractIterator &operator=(const BlockStore::AbstractIterator &it) override;
  bool operator<(const BlockStore::AbstractIterator &it) override;
  bool operator>(const BlockStore::AbstractIterator &it) override;
  bool operator>=(const BlockStore::AbstractIterator &it) override;
  bool operator<=(const BlockStore::AbstractIterator &it) override;

  const std::vector<uint8_t> &operator*() override;

  BlockStore::AbstractIterator &operator++() override;
  //BlockStore::AbstractIterator &operator++(int) override;
  BlockStore::AbstractIterator &operator--() override;
  //BlockStore::AbstractIterator &operator--(int) override;

  BlockStore::AbstractIterator &operator+=(const int &n) override;
  BlockStore::AbstractIterator &operator-=(const int &n) override;
  BlockStore::AbstractIterator &operator-(const int &n) override;
  BlockStore::AbstractIterator &operator+(const int &n) override;
 private:
  BlockStoreFlat &bstore_;
  uint32_t n_;
  std::vector<uint8_t> value_;
};
}