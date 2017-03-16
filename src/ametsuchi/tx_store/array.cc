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

#include <ametsuchi/tx_store/array.h>
#include <ametsuchi/serializer.h>

namespace ametsuchi {
namespace tx_store {

Array::Array(const std::string &path)
  : file_(path), index_(path+"_index")/*, cache_(100000)*/{
  file_.open();
}

std::size_t Array::append(const ByteArray &data) {
  file_.append(data);
  auto n = index_.append(file_.size()) - 1;
  auto cache_data = data;
//  cache_.put(n, std::move(cache_data));
  return n;
}

ByteArray Array::get(const std::size_t n) {
//  auto data = cache_.get(n);
  /*if (data) {
    return *data;
  }*/
  auto offset_ = index_.get(n);
  size_t size = index_.get(n+1) - offset_;
  file_.seek(offset_);
  return file_.read(size);
}

}  // namespace tx_store
}  // namespace ametsuchi
