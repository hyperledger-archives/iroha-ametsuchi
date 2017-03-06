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
#include <ametsuchi/status.h>
#include <cstring>
#include <string>
#include <utility>
#include <vector>
#include "index.h"

namespace ametsuchi {
namespace table {

class Table {
 public:
  explicit Table(const std::string &path, uint64_t key_size,
                 uint64_t value_size);

  virtual bool putBatch(const std::vector<std::pair<ByteArray, ByteArray>> &batch);
  virtual bool put(const ByteArray &key, const ByteArray &value);
  virtual ByteArray &get(const ByteArray &key);
  virtual bool exists(const ByteArray &key);
  virtual bool remove(const ByteArray &key);
  virtual uint64_t size();

  // bidirectional iterator
  class Iterator;

  enum class Flags : uint8_t { Placeholder = 0, Valid = 1, Removed = 2 };

  struct Record;

 protected:
  Index index_;

  file::AppendableFile w_;
  file::SequentialFile r_;

  std::string path_;
};


class Table::Iterator {
 public:
  Iterator();
  ~Iterator();
  Iterator(const Iterator &it);
  Iterator(const Iterator &&it);
  void operator=(const Iterator &r);    // =
  void operator=(const Iterator &&r);   // =
  Iterator &operator++();               // postfix++
  Iterator &operator++(int);            // ++prefix
  Iterator &operator--();               // postfix--
  Iterator &operator--(int);            // --prefix
  Record &operator*();                  // dereference
  Record &operator->();                 // dereference
  bool operator==(const Iterator &it);  // ==
  bool operator<(const Iterator &it);   // <
  bool operator>(const Iterator &it);   // >
 protected:
  offset_t offset;
};

struct Table::Record {
  Table::Flags flags;
  ByteArray key;
  ByteArray value;

  Record(Table::Flags f, const ByteArray &k, const ByteArray &v)
      : flags(f), key(std::move(k)), value(std::move(v)) {}

  Record() : flags(table::Table::Flags::Placeholder), key(0), value(0) {}
};

}  // namespace table
}  // namespace ametsuchi

#endif  // AMETSUCHI_TABLE_H
