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

#include <ametsuchi/tx_store/index.h>

namespace ametsuchi {
namespace tx_store {

using file::offset_t;

Index::Index(const std::string &path) : Index(path, 100000){}

file::offset_t Index::get(std::size_t n) {
//  auto offset = cache_.get(n);
//  if (offset) {
//    return *offset;
//  }
  return inMemData.at(n);
  file_.seek(n * sizeof(offset_t));
  return *reinterpret_cast<offset_t*>(file_.read(sizeof(offset_t)).data());

}

std::size_t Index::append(file::offset_t offset) {
//  auto ptr = reinterpret_cast<const byte_t*>(&offset);
  // TODO remove ByteArray intermediate object
//  file_.append(ByteArray{ptr, ptr + sizeof(offset_t)});
//  cache_.put(size() - 1, std::move(offset));
  ++size_;
  inMemData.push_back(offset);
  return size_ - 1;
}

std::size_t Index::size() const {
  return size_;
}

void Index::set_cache_size(std::size_t cache_size) {
//  cache_.setMaxCacheSize(cache_size);
}

Index::Index(const std::string &path, const std::size_t inMemSize): file_(path)/*, cache_(inMemSize)*/ {
  file_.open();
  inMemData.reserve(inMemSize);
  // Append zero if the file is new
  if (!file_.size()){
    append(0);
  }
  size_ = 0;
}

}  // namespace tx_store
}  // namespace ametsuchi
