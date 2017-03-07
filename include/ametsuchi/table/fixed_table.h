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
#include <ametsuchi/file/file.h>

#include <string>
#include <algorithm>

namespace ametsuchi{
namespace table{

template<typename T>
class FixedTable {
 public:
  FixedTable(const std::string &path);

  void append(const T &data);

  void appendBatch(const std::vector<T> &data);

  T get(file::offset_t index);

  std::vector<T> getBatch(uint64_t num, file::offset_t index);

  void replace(const T& t, file::offset_t index);

 private:
  file::AppendableFile w_;
  file::SequentialFile r_;
};


// imp
template<typename T>
FixedTable<T>::FixedTable(const std::string &path) : w_(path), r_(path) {
  w_.open();
  r_.open();
}

template<typename T>
void FixedTable<T>::append(const T &data) {
  file::flag_t flag = 0;
  w_.append(std::vector<file::flag_t>{flag});
  w_.append(data);
}

template<typename T>
void FixedTable<T>::appendBatch(const std::vector<T> &data) {
  std::for_each(data.begin(), data.end(), [this](const auto &elem){
    this->append(elem);
  });
}

template<typename T>
T FixedTable<T>::get(file::offset_t index) {
  ByteArray ptr = r_.read(sizeof(T) + sizeof(file::flag_t), index * sizeof(T));
  // file::flag_t flag = ptr[0];
  // TODO: handle somehow flag
  T t = *(T*)&ptr[1];
  return t;
}

template<typename T>
std::vector<T> FixedTable<T>::getBatch(uint64_t num, file::offset_t index) {
  ByteArray array = r_.read((sizeof(T) + sizeof(file::flag_t)) * num, index * sizeof(T));
  // std::vector<file::flag_t> flags;
  std::vector<T> v;
  for (auto i = array.begin(); i != array.end(); i += sizeof(T) + sizeof(file::flag_t)) {
    // flags.push_back(*i);
    v.push_back(*(T*)((&*i) + 1));
  }
  return v;
}

template<typename T>
void FixedTable<T>::replace(const T& t, file::offset_t index) {
  // r_.
}

}
}

#endif  // AMETSUCHI_FIXED_TABLE_H
