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
#include <iostream>

namespace ametsuchi {
namespace tx_store {

Array::Array(const std::string &path)
  : file_(path), index_(path+"_index"), uncommitted_(), uncommitted_size_(0){
  file_.open();
}

std::size_t Array::append(const ByteArray &data) {
  size_t last = index_.size();
  auto offset = index_.get(last-1) + uncommitted_size_;
  file_.seek(offset);
  file_.write(data);
  uncommitted_.push_back(offset+data.size());
  uncommitted_size_ += data.size();
//  return index_.append(offset+data.size());
  return index_.size() - 1 + uncommitted_.size(); // to return the same value as it was before (commented above)
}

std::size_t Array::batch_append(const std::vector<ByteArray> &batch_data) {
  if (batch_data.size() == 0){
    return index_.size() - 1 + uncommitted_.size();
  }

  // define first offset to return the beginning of the batch
  auto it = batch_data.begin();
  size_t first_offset = append(*it);
  ++it;

  // append remaining byte arrays
  for (auto it = batch_data.begin(); it < batch_data.end(); ++it){
    append(*it);
  }

  return first_offset;
}

ByteArray Array::get(const std::size_t n) {
  auto offset_ = index_.get(n);
  auto next_offset = index_.get(n+1);
  size_t size = next_offset - offset_;
  file_.seek(offset_);
  return file_.read(size);
}

void Array::commit() {
  for (auto offset: uncommitted_){
    index_.append(offset);
  }
  uncommitted_.clear();
  uncommitted_size_ = 0;
}

void Array::rollback() {
  uncommitted_.clear();
  uncommitted_size_ = 0;
}

}  // namespace tx_store
}  // namespace ametsuchi
