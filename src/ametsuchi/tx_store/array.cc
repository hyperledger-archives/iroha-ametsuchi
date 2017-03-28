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

#include <ametsuchi/serializer.h>
#include <ametsuchi/tx_store/array.h>
#include <iostream>

namespace ametsuchi {
namespace tx_store {

Array::Array(const std::string &path)
    : file_(path),
      index_(path + "_index"),
      uncommitted_(),
      commit_size(0) {

  if (!file_.open())
  {
    //TODO: handle exception

  }
}

std::size_t Array::append(const ByteArray &data) {

  auto size = data.size();
  auto ptr = reinterpret_cast<byte_t *>(&size);

  std::copy(ptr, ptr+sizeof(std::size_t), std::back_inserter(uncommitted_)); // the same as push back in the loop
  std::copy(data.begin(), data.end(), std::back_inserter(uncommitted_));
  commit_size += data.size()+sizeof(std::size_t);
  // Update index offset
  return index_.append(commit_size);

}

std::size_t Array::append_batch(const std::vector<ByteArray> &batch_data) {
  if (batch_data.size() == 0) {
    return index_.size() - 1 + uncommitted_.size();
  }
  std::size_t first_offset;
  // gets offset to the first byte array from batch
  auto iter = batch_data.begin();
  first_offset = append(*iter);
  ++iter;

  for (;iter < batch_data.end(); ++iter) {
    append(*iter);
  }
  return first_offset;
}

ByteArray Array::get(const std::size_t n) {
  auto offset_ = index_.get(n) + sizeof(std::size_t);
  auto next_offset = index_.get(n + 1);
  size_t size = next_offset - offset_;
  file_.seek(offset_);
  return file_.read(size);
}

void Array::commit() {
  file_.append(uncommitted_.data(), commit_size);
  index_.commit();

  uncommitted_.clear();
  commit_size = 0;
}

void Array::rollback() {
  index_.rollback();
  uncommitted_.clear();
  commit_size = 0;
}

bool Array::is_committed() const
{
  return index_.is_committed() && uncommitted_.size() == 0;
}


}  // namespace tx_store
}  // namespace ametsuchi
