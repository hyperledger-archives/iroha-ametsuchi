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
#include <ametsuchi/table/field_table.h>

namespace ametsuchi {

/**
 * We define custom serializer for table::Field::Record
 * We need put, size.
 * Define serializer in ametsuchi::serialize namespace.
 */
namespace serialize {

using Record = table::Field::Record;

template <>
inline size_t size<Record>(const Record &r) {
  return size(r.flag) + size(r.data);
}
}  // namespace serialize

namespace table {

Field::Field(const std::string &p) : f_(p), file_size(0) {}


Field::~Field() {}


offset_t Field::append(const ByteArray &data) {
  // current size = offset to new record
  size_t offset = f_.size();

  // construct record
  Record record{Record::VALID, data};

  // buffer to save record
  ByteArray buf(serialize::size(record));
  uint8_t * ptr = buf.data();

  // put record to buffer
  serialize::put(ptr, record.flag);
  serialize::put(ptr, record.data);

  // append buffer
  f_.append(buf);

  return offset;  // offset to new value
}


ByteArray Field::get(const offset_t offset) {
  if (offset > f_.size()) return ByteArray{0};  // wrong offset

  f_.seek(offset);

  Record    r;
  ByteArray b_flag = f_.read(sizeof(flag_t));
  if (b_flag.size() != 1) {
    return ByteArray{0};  // wrong offset or no data
  }

  r.flag = b_flag[0];

  switch (r.flag) {
    case table::Field::Record::REMOVED: {
      return ByteArray{0};  // byte array of size 0
    }
    case table::Field::Record::VALID: {
      // read length
      ByteArray     b_length = f_.read(sizeof(size_t));
      const byte_t *ptr      = b_length.data();
      // deserialize length
      uint64_t length;
      serialize::get(&length, ptr);

      // read data
      ByteArray data = f_.read(length);

      return data;
    }
    default: {
      return ByteArray{0};  // wrong offset or no data
    }
  }
}


bool Field::remove(const offset_t offset) {
  if (offset > f_.size()) return false;
  f_.seek(offset);
  f_.write(ByteArray({table::Field::Record::REMOVED}));
}

bool Field::update(const offset_t offset, const ByteArray &new_value) {
  if (offset > f_.size()) return false;
  remove(offset);
  append(new_value);
}

std::string Field::path() { return path_; }



//////////////////////////
/// Field::ForwardIterator
Field::ForwardIterator Field::begin() { return ForwardIterator(*this); }


Field::ForwardIterator Field::end() {
  return ForwardIterator(*this, f_.size());
}


Field::ForwardIterator::ForwardIterator(Field &ft) : ft_(ft), offset_(0) {
  value_ = ft_.get(offset_);
}


Field::ForwardIterator::ForwardIterator(Field &ft, offset_t offset)
    : ft_(ft), offset_(offset) {
  value_ = ft_.get(offset);
}


Field::ForwardIterator::ForwardIterator(const Field::ForwardIterator &it)
    : ft_(it.ft_), offset_(it.offset_), value_(it.value_) {}


Field::ForwardIterator &Field::ForwardIterator::operator++() {
  // bad solution. better is to std::serialize(Record). But do we need to create
  // Record object?
  offset_ += serialize::size(value_) + sizeof(flag_t);
  value_ = ft_.get(offset_);
  return *this;
}


Field::ForwardIterator Field::ForwardIterator::operator++(int) {
  ForwardIterator iterator(*this);
  // bad solution. better is to std::serialize(Record). But do we need to create
  // Record object?
  offset_ += serialize::size(value_) + sizeof(flag_t);
  value_ = ft_.get(offset_);
  return iterator;
}


ByteArray &Field::ForwardIterator::operator*() { return value_; }


bool Field::ForwardIterator::operator==(const Field::ForwardIterator &it) {
  return offset_ == it.offset_;
}


bool Field::ForwardIterator::operator<(const Field::ForwardIterator &it) {
  return offset_ < it.offset_;
}


bool Field::ForwardIterator::operator>(const Field::ForwardIterator &it) {
  return offset_ > it.offset_;
}

}  // namespace table
}  // namespace ametsuchi
