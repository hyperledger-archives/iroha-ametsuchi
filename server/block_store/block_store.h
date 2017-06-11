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

#include <string>
#include <vector>

namespace block_store {

class BlockStore {
 public:
  virtual void add(uint32_t id, const std::vector<uint8_t> &block) = 0;
  virtual const std::vector<uint8_t> get(uint32_t id) = 0;
  virtual const uint32_t last_id() = 0;
  virtual void remove(uint32_t id) = 0;

  class AbstractIterator;
  class Iterator;

  virtual BlockStore::Iterator begin() = 0;
  virtual BlockStore::Iterator end() = 0;
};


class BlockStore::AbstractIterator{

 public:

  // Operators
  virtual bool operator==(const BlockStore::AbstractIterator &it)=0;
  virtual bool operator!=(const BlockStore::AbstractIterator &it) = 0;
  virtual BlockStore::AbstractIterator& operator=(const BlockStore::AbstractIterator &it) = 0;
  virtual bool operator<(const BlockStore::AbstractIterator &it) = 0;
  virtual bool operator>(const BlockStore::AbstractIterator &it) = 0;
  virtual bool operator>=(const BlockStore::AbstractIterator &it) = 0;
  virtual bool operator<=(const BlockStore::AbstractIterator &it) = 0;

  virtual const std::vector<uint8_t> &operator*() = 0;

  virtual BlockStore::AbstractIterator& operator++() = 0;
 // virtual BlockStore::AbstractIterator& operator++(int) = 0;
  virtual BlockStore::AbstractIterator& operator--() = 0;
 // virtual BlockStore::AbstractIterator& operator--(int) = 0;

  virtual BlockStore::AbstractIterator& operator+=(const int &n) = 0;
  virtual BlockStore::AbstractIterator& operator-=(const int &n) = 0;
  virtual BlockStore::AbstractIterator& operator-(const int &n) = 0;
  virtual BlockStore::AbstractIterator& operator+(const int &n) = 0;

};

class BlockStore::Iterator {
 private:
  BlockStore::AbstractIterator * baseImpl;
 public:
  Iterator(BlockStore::AbstractIterator* b) : baseImpl(b) {}
  bool operator==(const BlockStore::Iterator &it) {
    return baseImpl->operator==(*it.baseImpl);
  };
  bool operator!=(const  BlockStore::Iterator &it){
    return baseImpl->operator!=(*it.baseImpl);
  };
  BlockStore::Iterator& operator=(const Iterator &it){
    baseImpl->operator=(*it.baseImpl);
    return *this;
  };
  bool operator<(const BlockStore::Iterator &it){
    return baseImpl->operator<(*it.baseImpl);
  };
  bool operator>(const BlockStore::Iterator &it) {
    return baseImpl->operator<(*it.baseImpl);
  };
  bool operator>=(const BlockStore::Iterator &it){
    return baseImpl->operator>=(*it.baseImpl);
  };
  bool operator<=(const BlockStore::Iterator &it){
    return baseImpl->operator<=(*it.baseImpl);
  };

  const std::vector<uint8_t> &operator*() {
    return baseImpl->operator*();
  };

  BlockStore::Iterator& operator++() {
    baseImpl->operator++();
    return *this;
  };
  BlockStore::Iterator& operator++(int) {
    baseImpl->operator++();
    return *this;
  };
  BlockStore::Iterator& operator--() {
    baseImpl->operator--();
    return *this;
  };
  BlockStore::Iterator& operator--(int) {
    baseImpl->operator--();
    return *this;
  };

  BlockStore::Iterator& operator+=(const int &n) {
    baseImpl->operator+=(n);
    return *this;
  };

  BlockStore::Iterator& operator-=(const int &n) {
    baseImpl->operator-=(n);
    return *this;
  };
  BlockStore::Iterator& operator-(const int &n){
    baseImpl->operator-(n);
    return *this;
  };
  BlockStore::Iterator& operator+(const int &n){
    baseImpl->operator+(n);
    return *this;
  };
};


}
