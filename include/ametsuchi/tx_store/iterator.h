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

#ifndef AMETSUCHI_ITERATOR_H
#define AMETSUCHI_ITERATOR_H

#include <ametsuchi/tx_store/array.h>

namespace ametsuchi {
namespace tx_store {

class Array::RandomAccessIterator {
 public:
  RandomAccessIterator(Array &array);
  RandomAccessIterator(Array &array, std::size_t offset);
  RandomAccessIterator(const RandomAccessIterator &it);
  bool operator==(const Array::RandomAccessIterator &it);
  bool operator<(const Array::RandomAccessIterator &it);
  bool operator>(const Array::RandomAccessIterator &it);
  ByteArray &operator*();
  RandomAccessIterator operator++();
  RandomAccessIterator operator++(int);
  RandomAccessIterator operator--();
  RandomAccessIterator operator--(int);
  RandomAccessIterator operator+=(const int &n);
  RandomAccessIterator operator-=(const int &n);
  RandomAccessIterator operator-(const int &n);
  RandomAccessIterator operator+(const int &n);

 protected:
  Array &array_;
  std::size_t n_;
  ByteArray value_;
};

}  // namespace tx_store
}  // namespace ametsuchi

#endif  // AMETSUCHI_ITERATOR_H
