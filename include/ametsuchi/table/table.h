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

#ifndef AMETSUCHI_TABLE_H
#define AMETSUCHI_TABLE_H

#include <ametsuchi/file/file.h>
#include <ametsuchi/globals.h>
#include <ametsuchi/serializer.h>
#include <cstring>
#include <string>

namespace ametsuchi {

namespace table {

using tag_t = uint64_t;
using flag_t = uint8_t;
using length_t = uint32_t;

class Table {
 public:
  explicit Table(const std::string &path);

 private:
  enum class Flags : flag_t { Removed, Valid };

  file::AppendableFile file_;
};

struct Record {
  tag_t tag;
  flag_t flags;  // look at Table::Flags
  length_t length;
  ByteArray blob;

  Record(tag_t t, flag_t f, length_t l, ByteArray b)
      : tag(t), flags(f), length(l), blob(std::move(b)) {}

  Record() : tag(0), flags(0), length(0), blob(0) {}
};

}  // namespace table

namespace serialize {
/**
 * Serializer/deserializer for Record.
 * Should as effective as possible.
 */

using Record = table::Record;
template <>
class Serializer<Record> {
 public:
  ByteArray serialize(const Record *r) {
    using table::tag_t;
    using table::flag_t;
    using table::length_t;

    size_t total = sizeof(tag_t) + sizeof(flag_t) + sizeof(length_t) +
                   (sizeof(ByteArray::value_type) * r->blob.size());

    ByteArray ret(total);

    ByteArray::value_type *ptr = ret.data();

    // serialize tag
    *reinterpret_cast<tag_t *>(ptr) = r->tag;
    ptr += sizeof(tag_t);

    // serialize flags
    *reinterpret_cast<flag_t *>(ptr) = r->flags;
    ptr += sizeof(flag_t);

    // serialize length
    *reinterpret_cast<length_t *>(ptr) = r->length;
    ptr += sizeof(length_t);

    // serialize bytes
    memcpy(ptr, reinterpret_cast<const ByteArray::value_type *>(r->blob.data()),
           (sizeof(ByteArray::value_type) * r->blob.size()));

    return ret;
  }

  Record deserialize(const ByteArray *b) {
    using table::tag_t;
    using table::flag_t;
    using table::length_t;

    Record ret;
    const ByteArray::value_type *ptr = b->data();

    memcpy(&ret.tag, ptr, sizeof(tag_t));
    ptr += sizeof(tag_t);

    memcpy(&ret.flags, ptr, sizeof(flag_t));
    ptr += sizeof(flag_t);

    memcpy(&ret.length, ptr, sizeof(length_t));
    ptr += sizeof(length_t);

    size_t n = (sizeof(ByteArray::value_type) * ret.length);
    ret.blob = ByteArray{ptr, ptr + n};

    return ret;
  }
};


}  // namespace serialize
}  // namespace ametsuchi

#endif  // AMETSUCHI_TABLE_H
