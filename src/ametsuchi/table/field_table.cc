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

// TODO(warchant): pass RWFileSafe instead of path?
FieldTable::FieldTable(const std::string &p)
    : f_(new file::RWFile(p)), file_size(0) {
  if (!f_->open()) throw exception::IOError("FieldTable::" + p);
}


FieldTable::~FieldTable() {}


file::offset_t FieldTable::append(const ByteArray &data) {
  // TODO(warchant): return Status object
  // current size = offset to new record
  file::offset_t offset = f_->size();

  constexpr auto header_size = sizeof(Flag) + sizeof(uint64_t);
  ByteArray buf(header_size + data.size());
  byte_t *ptr = buf.data();

  serialize::put(ptr, Flag::VALID);
  serialize::put(ptr, data);

  f_->append(buf);

  return offset;  // offset to new value
}


ByteArray FieldTable::get(const file::offset_t offset) {
  // TODO(warchant): return Status object
  if (offset > (file::offset_t)f_->size()) return EMPTY;  // wrong offset

  f_->seek(offset);

  constexpr auto header_size = sizeof(file::flag_t) + sizeof(uint64_t);
  ByteArray header = f_->read(header_size);

  if (header.size() != header_size) {
    return EMPTY;  // wrong offset or no data
  }

  const byte_t *ptr = header.data();

  Flag flag;
  serialize::get(&flag, ptr);

  switch (flag) {
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
  // TODO(warchant): return Status object
  if (offset > (file::offset_t)f_->size()) return false;
  f_->seek(offset);
  return sizeof(file::flag_t) == f_->write(ByteArray{Flag::REMOVED});
}

file::offset_t FieldTable::update(const file::offset_t offset,
                                  const ByteArray &new_value) {
  // TODO(warchant): return Status object
  if (offset > (file::offset_t)f_->size()) {
    throw exception::IOError("offset > filesize");
  }
  remove(offset);
  return append(new_value);
}

std::string FieldTable::path() { return f_->get_path(); }


/**
 * 1. ensure we have enough on-disk memory to perform compaction
 */
void FieldTable::compact() {}


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
  offset_ += serialize::size(value_) + sizeof(file::flag_t) + sizeof(uint64_t);
  value_ = ft_.get(offset_);
  return *this;
}


FieldTable::ForwardIterator FieldTable::ForwardIterator::operator++(int) {
  ForwardIterator iterator(*this);
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
