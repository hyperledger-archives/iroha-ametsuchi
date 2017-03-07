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

#include <ametsuchi/table/field_table.h>

namespace ametsuchi {
namespace table {

Field::Field(const std::string &p) : rw_(p), file_size(0) {
  // get file length
  ra_.seekEnd();
  file_size = ra_.position();

  ra_.open();
  r_.open();
}


Field::~Field() {
  ra_.close();
  r_.close();
}


offset_t Field::append(const ByteArray &value) {
  size_t old_length = file_size;

  // append
  // flag | length | bytes
  ra_.append(file::Flag::EXISTS);
  ra_.append(value.size());
  ra_.append(value);

  file_size = ra_.position();

  return old_length;  // offset to new value
}


ByteArray Field::get(const offset_t offset) {
  ByteArray flag = r_.read(1, offset);
  switch (flag[0]) {
    case file::Flag::EXISTS:
      return r_.readByteArray(offset + sizeof(file::flag_t));
    case file::Flag::REMOVED:
      return nullptr;
    default:
      return nullptr;
  }
}


bool Field::update(const offset_t offset, const ByteArray &new_value) {
  ra_.seek(offset);

}

bool Field::remove(const offset_t offset) {
  ra_.seek(offset);
  ra_.append(file::Flag::REMOVED);
  ra_.seekEnd();
}


Field::ForwardIterator Field::begin() {
  return ForwardIterator(*this);
}


Field::ForwardIterator Field::end() {
  return ForwardIterator(*this, file_size);
}


Field::ForwardIterator::ForwardIterator(Field &ft) : ft_(ft) {
  offset_ = 0;
  value_  = ft_.get(offset_);
}


Field::ForwardIterator::ForwardIterator(Field &ft, offset_t offset)
    : ft_(ft), offset_(offset) {
  value_ = ft_.get(offset);
}


Field::ForwardIterator::ForwardIterator(
    const Field::ForwardIterator &it)
    : ft_(it.ft_), offset_(it.offset_), value_(it.value_) {}


Field::ForwardIterator &Field::ForwardIterator::operator++() {
  offset_ +=
      value_.size() + 8;  // size of current value + 8 for length of value
  value_ = ft_.get(offset_);
  return *this;
}


Field::ForwardIterator Field::ForwardIterator::operator++(int) {
  ForwardIterator iterator(*this);
  offset_ +=
      value_.size() + 8;  // size of current value + 8 for length of value
  value_ = ft_.get(offset_);
  return iterator;
}


ByteArray &Field::ForwardIterator::operator*() { return value_; }


bool Field::ForwardIterator::operator==(
    const Field::ForwardIterator &it) {
  return offset_ == it.offset_;
}


bool Field::ForwardIterator::operator<(
    const Field::ForwardIterator &it) {
  return offset_ < it.offset_;
}


bool Field::ForwardIterator::operator>(
    const Field::ForwardIterator &it) {
  return offset_ > it.offset_;
}

}  // namespace table
}  // namespace ametsuchi
