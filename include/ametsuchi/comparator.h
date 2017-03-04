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

#ifndef AMETSUCHI_COMPARATOR_H
#define AMETSUCHI_COMPARATOR_H

#include <string.h>
#include "globals.h"

namespace ametsuchi {

template <typename T>
class Comparator {
 public:
  virtual int operator()(const T& a, const T& b) = 0;
};

/**
 * Compares two byte arrays as two unsigned integers, it means that:
 *
 * 0x000001 == 0x01
 * 0x000000 <  0x02
 * 0x02     <  0x0101
 *
 * comparison of two arrays of the same size will be performed in
 * lexicographical order.
 *
 * @returns if a>b then -1; if a == b then 0; if a < b then +1
 *
 * Must be as effective as possible.
 */
class ByteArrayCmp : public Comparator<ByteArray> {
 public:
  inline int operator()(const ByteArray& a, const ByteArray& b) {
    size_t asize = a.size();
    size_t bsize = b.size();

    if (asize == bsize) {
      return memcmp(a.data(), b.data(), asize);
    } else if (asize < bsize) {
      // if prefix(b) = n zeros, skip them
      size_t n = bsize - asize;
      for (size_t i = 0; i < n; i++)
        if (b[i] != 0) return -1;
      return memcmp(a.data(), b.data() + n, asize);
    } else {  // asize > bsize
      // if prefix(a) = n zeros, skip them
      size_t n = asize - bsize;
      for (size_t i = 0; i < n; i++)
        if (a[i] != 0) return +1;
      return memcmp(a.data() + n, b.data(), bsize);
    }
  }
};
}

#endif  // AMETSUCHI_COMPARATOR_H
