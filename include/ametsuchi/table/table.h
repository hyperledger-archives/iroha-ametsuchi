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

#pragma once

#include <ametsuchi/file/file.h>
#include <ametsuchi/globals.h>
#include <ametsuchi/serializer.h>

namespace ametsuchi {
namespace table {

static const auto EMPTY = ByteArray{0};

enum Flag : file::flag_t {
  REMOVED = 1,
  VALID   = 2
  // rest is reserved for future use
};

template<typename T>
struct BaseRecord {
  file::flag_t flag;
  T data;
};

}

/**
 * Table specific serialization
 */
namespace serialize {

template<typename T>
using Record = table::BaseRecord<T>;

template <typename T>
inline size_t size(const Record<T> &r) {
  return size(r.flag) + size(r.data);
}

// Names of the following methods should be changed
// as in serializer.hpp with the resolving #33
template<typename T>
inline Record<T> getRecord(const ByteArray &src) {
  return *(Record<T>*)src.data();
}

template<typename T>
inline void putRecord(ByteArray &dst, Record<T> &r) {
  byte_t *ptr = dst.data();
  put(ptr, r.data);
}

template<typename T>
inline void putRecord(ByteArray &dst, Record<T> &&r) {
  byte_t *ptr = dst.data();
  put(ptr, r.data);
}

}
}
