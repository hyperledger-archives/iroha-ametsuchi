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
      uncommitted_size_(0) {
  if (!file_.open())
  {
    //TODO: handle exception
  }
}

std::size_t Array::append(const ByteArray &data) {
  /*
  size_t last = index_.size();
  auto offset = index_.get(last - 1) + uncommitted_size_;
  file_.seek(offset);
  file_.write(data);
  */

  //auto offset = index_.get_last();
  auto ptr = reinterpret_cast<byte_t *>(data.size());
  std::copy(ptr, ptr+sizeof(std::size_t), std::back_inserter(uncommitted_));

  //uncommitted_.push_back(ptr);
  std::move(data.begin(), data.end(), std::back_inserter(uncommitted_));
  uncommitted_size_ += data.size()+sizeof(std::size_t);
  return index_.append(uncommitted_size_);

}

std::size_t Array::append_batch(const std::vector<ByteArray> &batch_data) {
  if (batch_data.size() == 0) {
    return index_.size() - 1 + uncommitted_.size();
  }
  std::size_t last;
  std::for_each(batch_data.begin(), batch_data.end(), [](const auto & e){
    uncommitted_.push_back(std::move(e));
    uncommitted_size_ += e.size();
    last = index_.append(e.size());
  });
  // append remaining byte arrays (without committing)
  return last;
}

ByteArray Array::get(const std::size_t n) {
  auto offset_ = index_.get(n);
  auto next_offset = index_.get(n + 1);
  size_t size = next_offset - offset_;
  file_.seek(offset_);
  return file_.read(size);
}

void Array::commit() {
  file_.append(uncommitted_.data(), uncommitted_size_);

  uncommitted_
  // index
  index.commit();
  //index_.append_batch(uncommitted_);
  uncommitted_.clear();
  uncommitted_size_ = 0;
}

void Array::rollback() {
  index_.rollback();
  uncommitted_.clear();
  uncommitted_size_ = 0;
}

bool Array::is_committed() const
{
  return uncommitted_.size() == 0;
}

}  // namespace tx_store
}  // namespace ametsuchi
