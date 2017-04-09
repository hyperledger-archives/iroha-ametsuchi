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

#include <ametsuchi/exception.h>
#include <ametsuchi/globals.h>
#include <cmath>

namespace ametsuchi {
namespace buffer {

/**
 * Basic circular buffer
 */
template <typename T>
class CircularBuffer {
 public:
  CircularBuffer(size_t);
  CircularBuffer(CircularBuffer &&);
  // Used only in narrow merkle tree
  // and here does need to be copied
  // deleted for preventing errors
  CircularBuffer(const CircularBuffer &) = delete;
  ~CircularBuffer();

  /**
   * Performs reallocation of the data
   * so the buffer able to store provided
   * number of elems
   * O(n)
   */
  void resize(size_t);

  /**
   * O(1)
   * Add element to the buffer end pointer
   */
  T push(T &&);

  /**
   * O(1)
   * Add element to the buffer end pointer
   */
  T push(const T &);
  /**
   * O(1)
   * @param n elemens for removing
   */
  void pop(size_t n = 1);


  /**
   * O(1)
   * Accessing element by front.
   */
  T &front();
  T front() const;

  /**
   * O(1)
   * Accessing element by back.
   */
  T &back();
  T back() const;


  /**
   * O(1)
   * Accessing element by index
   */
  T &operator[](size_t);
  T operator[](size_t) const;

  /**
   * Number of elements
   */
  constexpr size_t size() const { return sz; }
  /**
   * Maximum storage capacity
   */
  constexpr size_t capacity() const { return cap; }

  constexpr bool is_full() const { return size() == capacity(); }

 private:
  // Raw data array
  T *v;
  // Storage capacity
  size_t cap;
  // Number of elements stored so far, can be in range [0;cap]
  size_t sz;
  // Index of after last element, can be in range [0;sz)
  size_t i_end;

  constexpr size_t start() const { return diff(i_end, size()); }

  // Determines n position before p
  constexpr size_t diff(size_t p, size_t n) const {
    return p < n ? p + capacity() - n : p - n;
  }

  constexpr size_t overflowed() { return capacity(); }

 public:
  class ForwardIter {
   public:
    ForwardIter(CircularBuffer<T> *, size_t);
    bool operator==(const ForwardIter &i) const;
    bool operator!=(const ForwardIter &i) const;
    ForwardIter &operator++();
    ForwardIter operator+(size_t t);
    ForwardIter &operator+=(size_t t);
    T &operator*();
    T &operator[](size_t t);
    size_t size() const {return cs->size();}
    size_t capacity() const {return cs->capacity();}
    ForwardIter &to_last() {
      pos = cs->diff(cs->i_end, 1);
      return *this;
    }

   private:
    CircularBuffer<T> *cs;
    size_t pos;

    size_t pos_inc(size_t t);
  };


  ForwardIter begin() { return ForwardIter(this, diff(i_end, size())); }

  ForwardIter end() { return ForwardIter(this, overflowed()); }

  ForwardIter last() { return ForwardIter(this, diff(i_end, 1)); }

 private:
};

#include "circular_buffer.inc"

}  // namespace buffer
}  // namespace ametsuchi
