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

FieldTable::FieldTable(const std::string &p)
    : file_size(0), f_(new file::RWFile(p)) {
  if (!f_->open()) throw exception::IOError("FieldTable::" + p);
}


FieldTable::~FieldTable() {}


file::offset_t FieldTable::append(const ByteArray &data) {
  // current size = offset to new record
  size_t offset = f_->size();
  ByteArray buf;
  serialize::putRecord(buf, Record{Flag::VALID, data});

  f_->append(buf);

  return offset;  // offset to new value
}


ByteArray FieldTable::get(const file::offset_t offset) {
  if (offset > f_->size()) return EMPTY;  // wrong offset

  f_->seek(offset);

  Record r;
  ByteArray header = f_->read(sizeof(file::flag_t) + sizeof(uint64_t));

  if (header.size() != 9) {
    return EMPTY;  // wrong offset or no data
  }

  const byte_t *ptr = header.data();
  serialize::get(&r.flag, ptr);

  switch (r.flag) {
    case Flag::REMOVED: {
      return EMPTY;  // byte array of size 0
    }
    case Flag::VALID: {
      uint64_t length;
      serialize::get(&length, ptr);

      // read data
      ByteArray data = f_->read(length);

      return data;
    }
    default: {
      return EMPTY;  // wrong offset or no data
    }
  }
}


bool FieldTable::remove(const file::offset_t offset) {
  if (offset > f_->size()) return false;
  f_->seek(offset);
  return sizeof(Record) == f_->write(ByteArray{Flag::REMOVED});
}

file::offset_t FieldTable::update(const file::offset_t offset,
                                  const ByteArray &new_value) {
  if (offset > f_->size()) return false;
  remove(offset);
  return append(new_value);
}

std::string FieldTable::path() { return f_->get_path(); }


/**
 * 1. ensure we have enough on-disk memory to perform compaction
 */
void FieldTable::compact() {

}


//////////////////////////
/// FieldTable::ForwardIterator
FieldTable::ForwardIterator FieldTable::begin() {
  return ForwardIterator(*this);
}


FieldTable::ForwardIterator FieldTable::end() {
  return ForwardIterator(*this, f_->size());
}


FieldTable::ForwardIterator::ForwardIterator(FieldTable &ft)
    : ft_(ft), offset_(0) {
  value_ = ft_.get(offset_);
}


FieldTable::ForwardIterator::ForwardIterator(FieldTable &ft,
                                             file::offset_t offset)
    : ft_(ft), offset_(offset) {
  value_ = ft_.get(offset);
}


FieldTable::ForwardIterator::ForwardIterator(
    const FieldTable::ForwardIterator &it)
    : ft_(it.ft_), offset_(it.offset_), value_(it.value_) {}


FieldTable::ForwardIterator &FieldTable::ForwardIterator::operator++() {
  // bad solution. better is to std::serialize(Record). But do we need to create
  // Record object?
  offset_ += serialize::size(value_) + sizeof(file::flag_t) + sizeof(uint64_t);
  value_ = ft_.get(offset_);
  return *this;
}


FieldTable::ForwardIterator FieldTable::ForwardIterator::operator++(int) {
  ForwardIterator iterator(*this);
  // bad solution. better is to std::serialize(Record). But do we need to create
  // Record object?
  offset_ += serialize::size(value_) + sizeof(file::flag_t) + sizeof(uint64_t);
  value_ = ft_.get(offset_);
  return iterator;
}


ByteArray &FieldTable::ForwardIterator::operator*() { return value_; }


bool FieldTable::ForwardIterator::operator==(
    const FieldTable::ForwardIterator &it) {
  return offset_ == it.offset_;
}


bool FieldTable::ForwardIterator::operator<(
    const FieldTable::ForwardIterator &it) {
  return offset_ < it.offset_;
}


bool FieldTable::ForwardIterator::operator>(
    const FieldTable::ForwardIterator &it) {
  return offset_ > it.offset_;
}

}  // namespace table
}  // namespace ametsuchi
