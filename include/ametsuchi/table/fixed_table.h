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

  T get(file::offset_t offset);

  std::vector<T> getBatch(uint64_t num, file::offset_t offset);

  void replace(T t, file::offset_t offset);

 private:
  std::unique_ptr<file::AppendableFile> w_;
  std::unique_ptr<file::SequentialFile> r_;
};


// imp
template<typename T>
FixedTable<T>::FixedTable(const std::string &path) :
  w_(std::make_unique<file::AppendableFile>(path)),
  r_(std::make_unique<file::SequentialFile>(path)) {
w_->open();
r_->open();
}

template<typename T>
void FixedTable<T>::append(const T &data) {
uint8_t flag = 0;
w_->append((std::vector<uint8_t>){flag});
w_->append(data);
}

template<typename T>
void FixedTable<T>::appendBatch(const std::vector<T> &data) {
std::for_each(data.begin(), data.end(), [this](const auto &elem){
    append(elem);
});
}

template<typename T>
T FixedTable<T>::get(file::offset_t offset) {
return *(T*)r_->read(sizeof(T), offset).data();
}

template<typename T>
std::vector<T> FixedTable<T>::getBatch(uint64_t num, file::offset_t offset) {
auto array = r_->read(sizeof(T) * num, offset);
return std::vector<T>(array.begin(), array.end());
}

template<typename T>
void FixedTable<T>::replace(T t, file::offset_t offset) {
// TODO: requires random file implementation
}

}
}

#endif  // AMETSUCHI_FIXED_TABLE_H
