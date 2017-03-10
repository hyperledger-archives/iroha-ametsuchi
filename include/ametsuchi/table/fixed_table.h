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
  class ForwardIterator;

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

// imp
template <typename T>
FixedTable<T>::FixedTable(const std::string &path) : file(path) {
  file.open();
}

template <typename T>
file::offset_t FixedTable<T>::append(const T &data) {
  return appendBatch(std::vector<T>{data});
}

template <typename T>
file::offset_t FixedTable<T>::append(const T &&data) {
  T t = std::move(data);
  return append(t);
}

template <typename T>
file::offset_t FixedTable<T>::appendBatch(const std::vector<T> &data) {
  file::offset_t num = recordNum();
  write(data);
  return num;
}

template <typename T>
T FixedTable<T>::get(file::offset_t index) {
  seek(index);
  ByteArray buf = file.read(serialize::size<Record>());
  T t = serialize::getRecord<T>(buf.data()).data;
  return t;
}

template <typename T>
std::vector<T> FixedTable<T>::getBatch(uint64_t num, file::offset_t index) {
  seek(index);
  constexpr size_t size = serialize::size<Record>();
  ByteArray buf = file.read(size * num);
  if (buf.size() != size * num) {
    // read < requested
    // (?) guess need to throw smth here
    num = buf.size() / size;
  }

  std::vector<T> v(num);
  auto ptr = reinterpret_cast<const Record*>(&*buf.begin());
  std::transform(ptr, ptr + num, v.begin(), [](const Record &in) {
    return serialize::getRecord<T>((const void*)&in).data;
  });

  return v;
}

template <typename T>
void FixedTable<T>::replace(const T &data, file::offset_t index) {
  remove(index);
  seek(index);
  write(std::vector<T>{data});
}

template <typename T>
void FixedTable<T>::remove(file::offset_t index) {
  setFlag(index, Flag::REMOVED);
}

template <typename T>
file::flag_t FixedTable<T>::getFlag(file::offset_t index) {
  seek(index);
  ByteArray buf = file.read(sizeof(file::flag_t));
  if (buf.size() == 0) return Flag::INVALID;
  return *(file::flag_t *)buf.data();
}

template <typename T>
void FixedTable<T>::setFlag(file::offset_t index, file::flag_t flag) {
  seek(index);
  file.write(ByteArray{flag});
}

template <typename T>
size_t FixedTable<T>::recordNum() {
  file.seek_to_end();
  return idx(file.position());
}

template <typename T>
void FixedTable<T>::write(const std::vector<T> &data) {
  ByteArray buf;
  for (const auto &elem : data) {
    serialize::putRecord(buf, Record{Flag::VALID, elem});
  }

  auto written = file.write(buf);
  assert(written == data.size() * sizeof(Record));
}

}
}

#endif  // AMETSUCHI_FIXED_TABLE_H
