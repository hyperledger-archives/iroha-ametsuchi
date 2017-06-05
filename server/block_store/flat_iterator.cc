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

#include "flat_iterator.h"


block_store::BlockStoreFlat::Iterator::Iterator(
    block_store::BlockStoreFlat& bstore)
    : bstore_(bstore), n_(0) {}

block_store::BlockStoreFlat::Iterator::Iterator(
    block_store::BlockStoreFlat& bstore, int64_t offset)
    : bstore_(bstore), n_(offset) {}

block_store::BlockStoreFlat::Iterator::Iterator(
    const block_store::BlockStoreFlat::Iterator &it)
    : bstore_(it.bstore_), n_(it.n_), value_(it.value_) {}

const std::vector<uint8_t> &block_store::BlockStoreFlat::Iterator::operator*() {
  value_ = bstore_.get(n_);
  return value_;
}

bool block_store::BlockStoreFlat::Iterator::operator==(
    const block_store::BlockStoreFlat::Iterator &it) {
  return false;
}

bool block_store::BlockStoreFlat::Iterator::operator!=(
    const block_store::BlockStoreFlat::Iterator &it) {
  return n_ != it.n_;
}

block_store::BlockStoreFlat::Iterator block_store::BlockStoreFlat::Iterator::
operator=(const block_store::BlockStoreFlat::Iterator &it) {
  return *this;
}

bool block_store::BlockStoreFlat::Iterator::operator<(
    const block_store::BlockStoreFlat::Iterator &it) {
  return n_ < it.n_;
}

bool block_store::BlockStoreFlat::Iterator::operator>(const block_store::BlockStoreFlat::Iterator &it) {
  return n_ > it.n_;
}

bool block_store::BlockStoreFlat::Iterator::operator>=(const block_store::BlockStoreFlat::Iterator &it) {
  return n_ >= it.n_;
}

bool block_store::BlockStoreFlat::Iterator::operator<=(const block_store::BlockStoreFlat::Iterator &it) {
  return n_ <= it.n_;
}

block_store::BlockStoreFlat::Iterator block_store::BlockStoreFlat::Iterator::operator++() {
  n_ += 1;
  return *this;
}

block_store::BlockStoreFlat::Iterator block_store::BlockStoreFlat::Iterator::operator++(int) {
  n_ += 1;
  return *this;
}

block_store::BlockStoreFlat::Iterator block_store::BlockStoreFlat::Iterator::operator--() {
  n_ -= 1;
  return *this;
}

block_store::BlockStoreFlat::Iterator block_store::BlockStoreFlat::Iterator::operator--(int) {
  n_ -= 1;
  return *this;
}

block_store::BlockStoreFlat::Iterator block_store::BlockStoreFlat::Iterator::operator+=(const int &n) {
  n_ += n;
  return *this;
}

block_store::BlockStoreFlat::Iterator block_store::BlockStoreFlat::Iterator::operator-=(const int &n) {
  n_ -= n;
  return *this;
}

block_store::BlockStoreFlat::Iterator block_store::BlockStoreFlat::Iterator::operator-(const int &n) {
  n_ -= n;
  return *this;
}

block_store::BlockStoreFlat::Iterator block_store::BlockStoreFlat::Iterator::operator+(const int &n) {
  n_ += n;
  return *this;
}

block_store::BlockStoreFlat::Iterator block_store::BlockStoreFlat::begin() {
  return Iterator(*this, 1);
}


block_store::BlockStoreFlat::Iterator block_store::BlockStoreFlat::end() {
  return Iterator(*this, this->last_id());
}






















