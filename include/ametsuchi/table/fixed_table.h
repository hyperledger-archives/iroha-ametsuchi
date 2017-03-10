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

#ifndef AMETSUCHI_FIXED_TABLE_H
#define AMETSUCHI_FIXED_TABLE_H

#include <ametsuchi/globals.h>
#include <ametsuchi/table/table.h>
#include <ametsuchi/file/file.h>
#include <ametsuchi/serializer.h>

#include <algorithm>
#include <string>

namespace ametsuchi {
namespace table {

#define FILE_PREFIX 0

template <typename T>
class FixedTable {
 public:
  class BidirIterator;

  using Record = BaseRecord<T>;

  FixedTable(const std::string &path);

  file::offset_t append(const T &data);
  file::offset_t append(const T &&data);

  file::offset_t appendBatch(const std::vector<T> &data);

  T get(file::offset_t index);

  std::vector<T> getBatch(uint64_t num, file::offset_t index);

  void replace(const T &t, file::offset_t index);

  void remove(file::offset_t index);

  file::flag_t getFlag(file::offset_t index);
  void setFlag(file::offset_t index, file::flag_t flags);

  size_t recordNum();

  BidirIterator begin();
  BidirIterator end();

 private:

  void write(const std::vector<T>&);

  void seek(file::offset_t index) {
    file.seek(index * (sizeof(T) + sizeof(Flag)));
  }

  inline size_t idx(file::offset_t offset) {
    assert(offset % (sizeof(T) + sizeof(Flag)) == FILE_PREFIX);
    return (offset - FILE_PREFIX) / (sizeof(T) + sizeof(Flag));
  }
  file::ReadWriteFile file;
};


template <typename T>
class FixedTable<T>::BidirIterator {
 public:
  BidirIterator(FixedTable<T> &field);
  BidirIterator(FixedTable<T> &field, file::offset_t offset);
  BidirIterator(const BidirIterator &it);
  bool operator==(const BidirIterator &it);
  bool operator<(const BidirIterator &it);
  bool operator>(const BidirIterator &it);
  T operator*();
  BidirIterator &operator++();    // postfix++
  BidirIterator operator++(int);  // ++prefix
  BidirIterator &operator--();    // postfix--
  BidirIterator operator--(int);  // --prefix
 protected:
  FixedTable<T> &ft_;
  file::offset_t offset_;
};

}
}

#include "fixed_table.inc"

#endif  // AMETSUCHI_FIXED_TABLE_H
