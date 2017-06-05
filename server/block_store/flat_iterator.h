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

#include "block_store_flat.h"
namespace block_store{

class BlockStoreFlat::Iterator{

 public:
  Iterator(BlockStoreFlat& bstore);
  Iterator(BlockStoreFlat& bstore, int64_t offset);
  Iterator(const Iterator& it);
  // Operators
  bool operator==(const BlockStoreFlat::Iterator &it);
  bool operator!=(const BlockStoreFlat::Iterator &it);
  Iterator operator=(const BlockStoreFlat::Iterator &it);
  bool operator<(const BlockStoreFlat::Iterator &it);
  bool operator>(const BlockStoreFlat::Iterator &it);
  bool operator>=(const BlockStoreFlat::Iterator &it);
  bool operator<=(const BlockStoreFlat::Iterator &it);

  const std::vector<uint8_t> &operator*();

  Iterator operator++();
  Iterator operator++(int);
  Iterator operator--();
  Iterator operator--(int);

  Iterator operator+=(const int &n);
  Iterator operator-=(const int &n);
  Iterator operator-(const int &n);
  Iterator operator+(const int &n);

 protected:
  BlockStoreFlat& bstore_;
  int64_t n_;
  std::vector<uint8_t> value_;

};



}


