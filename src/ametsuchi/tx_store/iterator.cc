/**
 * Copyright Soramitsu Co., Ltd. 2016 All Rights Reserved.
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
#include <ametsuchi/tx_store/iterator.h>

namespace ametsuchi {
namespace tx_store {


///////
// Iterator

Array::RandomAccessIterator::RandomAccessIterator(Array &array)
    : ft_(array), offset_(0) {
  value_ = ft_.get(ft_.index_.get(offset_));

}

Array::RandomAccessIterator::RandomAccessIterator(Array &array,
                                                  std::size_t offset)
    :ft_(array), offset_(offset)  {
  value_ = ft_.get(ft_.index_.get(offset_));
}

Array::RandomAccessIterator::RandomAccessIterator(const RandomAccessIterator &it)
    :ft_(it.ft_), offset_(it.offset_), value_(it.value_) {}

ByteArray& Array::RandomAccessIterator::operator*() {
  return value_;
}

Array::RandomAccessIterator& Array::RandomAccessIterator::operator++() {
  RandomAccessIterator iterator(*this);
  offset_ += 1;
  value_ = ft_.get(ft_.index_.get(offset_));
  return iterator;
}

Array::RandomAccessIterator Array::RandomAccessIterator::operator++(int) {
  RandomAccessIterator iterator(*this);
  offset_ += 1;
  value_ = ft_.get(ft_.index_.get(offset_));
  return iterator;
}

Array::RandomAccessIterator& Array::RandomAccessIterator::operator--() {
  RandomAccessIterator iterator(*this);
  offset_ -= 1;
  value_ = ft_.get(ft_.index_.get(offset_));
  return iterator;
}

Array::RandomAccessIterator Array::RandomAccessIterator::operator--(int) {
  RandomAccessIterator iterator(*this);
  offset_ -= 1;
  value_ = ft_.get(ft_.index_.get(offset_));
  return iterator;
}

Array::RandomAccessIterator &Array::RandomAccessIterator::operator+=(const int& n) {
  RandomAccessIterator iterator(*this);
  offset_ += n;
  value_ = ft_.get(ft_.index_.get(offset_));
  return iterator;
}

Array::RandomAccessIterator &Array::RandomAccessIterator::operator-=(const int &n) {
  RandomAccessIterator iterator(*this);
  offset_ -= n;
  value_ = ft_.get(ft_.index_.get(offset_));
  return iterator;
}

Array::RandomAccessIterator Array::RandomAccessIterator::operator-(const int &n) {
  RandomAccessIterator iterator(*this);
  offset_ -= n;
  value_ = ft_.get(ft_.index_.get(offset_));
  return iterator;
}

Array::RandomAccessIterator Array::RandomAccessIterator::operator+(const int &n) {
  RandomAccessIterator iterator(*this);
  offset_ += n;
  value_ = ft_.get(ft_.index_.get(offset_));
  return iterator;
}

bool Array::RandomAccessIterator::operator<(const Array::RandomAccessIterator &it) {
  return offset_ < it.offset_;
}

bool Array::RandomAccessIterator::operator==(const Array::RandomAccessIterator &it) {
  return offset_ == it.offset_;
}

bool Array::RandomAccessIterator::operator>(const Array::RandomAccessIterator &it) {
  return offset_ > it.offset_;
}

Array::RandomAccessIterator Array::begin() {
  return RandomAccessIterator(*this);
}

Array::RandomAccessIterator Array::end() {
  return RandomAccessIterator(*this, file_.size());
}





}  // namespace tx_store
}  // namespace ametsuchi


