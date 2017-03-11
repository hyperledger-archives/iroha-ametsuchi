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
#include <iterator>

namespace ametsuchi {
namespace table {

static const auto EMPTY = ByteArray{};

enum Flag : file::flag_t {
  INVALID = 0,
  REMOVED = 1,
  VALID = 2
  // rest is reserved for future use
};

template <typename T>
struct PACKED_STRUCT BaseRecord {
  file::flag_t flag;
  T data;
};
}

/**
 * Table specific serialization
 */
namespace serialize {

template <typename T>
using Record = table::BaseRecord<T>;

template <typename T>
inline constexpr size_t size(const Record<T> &r) {
  return size(r.flag) + size(r.data);
}

// Names of the following methods should be changed
// as in serializer.hpp with the resolving #33
template <typename T>
inline Record<T> getRecord(const void *src) {
  NON_TRIVIAL_CHECK;
  return *(Record<T> *)src;
}

template <typename T>
inline void putRecord(ByteArray &dst, T r);

template <typename T>
inline void putRecord(ByteArray &dst, Record<T> &r) {
  putRecord(dst, std::forward<Record<T>>(r));
}

template <typename T>
inline void putRecord(ByteArray &dst, Record<T> &&r) {
  NON_TRIVIAL_CHECK;
  size_t size = serialize::size(r.data);
  auto &bytes = reinterpret_cast<byte_t(&)[size]>(r.data);
  dst.push_back(r.flag);
  dst.insert(dst.end(), bytes, bytes + size);
}

template <>
inline void putRecord(ByteArray &dst, Record<ByteArray> &r) {
  const auto size = serialize::size(r.data) + sizeof(file::flag_t);
  dst.resize(size);
  byte_t *ptr = dst.data();
  serialize::put(ptr, r.flag);
  serialize::put(ptr, r.data);
}

template <>
inline void putRecord(ByteArray &dst, Record<ByteArray> &&r) {
  Record<ByteArray> m = std::move(r);
  putRecord(dst, m);
}
}
}
