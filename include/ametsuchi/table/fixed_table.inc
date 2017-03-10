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


namespace ametsuchi {
namespace table {

template <typename T>
using Record = typename FixedTable<T>::Record;

// Fixed table implementation
template <typename T>
FixedTable<T>::FixedTable(const std::string &path) : file(path) {
  file.open();
}

template <typename T>
file::offset_t FixedTable<T>::append(const T &data) {
  return appendBatch(std::vector<T>{data});
}

template <typename T>
file::offset_t FixedTable<T>::append(const T &&data) {
  T t = std::move(data);
  return append(t);
}

template <typename T>
file::offset_t FixedTable<T>::appendBatch(const std::vector<T> &data) {
  file::offset_t num = recordNum();
  write(data);
  return num;
}

template <typename T>
T FixedTable<T>::get(file::offset_t index) {
  seek(index);
  ByteArray buf = file.read(serialize::size<Record>());
  T t = serialize::getRecord<T>(buf.data()).data;
  return t;
}

template <typename T>
std::vector<T> FixedTable<T>::getBatch(uint64_t num, file::offset_t index) {
  seek(index);
  constexpr size_t size = serialize::size<Record>();
  ByteArray buf = file.read(size * num);
  if (buf.size() != size * num) {
    // read < requested
    // (?) guess need to throw smth here
    num = buf.size() / size;
  }

  std::vector<T> v(num);
  auto ptr = reinterpret_cast<const Record*>(&*buf.begin());
  std::transform(ptr, ptr + num, v.begin(), [](const Record &in) {
    return serialize::getRecord<T>((const void*)&in).data;
  });

  return v;
}

template <typename T>
void FixedTable<T>::replace(const T &data, file::offset_t index) {
  remove(index);
  seek(index);
  write(std::vector<T>{data});
}

template <typename T>
void FixedTable<T>::remove(file::offset_t index) {
  setFlag(index, Flag::REMOVED);
}

template <typename T>
file::flag_t FixedTable<T>::getFlag(file::offset_t index) {
  seek(index);
  ByteArray buf = file.read(sizeof(file::flag_t));
  if (buf.size() == 0) return Flag::INVALID;
  return *(file::flag_t *)buf.data();
}

template <typename T>
void FixedTable<T>::setFlag(file::offset_t index, file::flag_t flag) {
  seek(index);
  file.write(ByteArray{flag});
}

template <typename T>
size_t FixedTable<T>::recordNum() {
  file.seek_to_end();
  return idx(file.position());
}

template <typename T>
void FixedTable<T>::write(const std::vector<T> &data) {
  ByteArray buf;
  for (const auto &elem : data) {
    serialize::putRecord(buf, Record{Flag::VALID, elem});
  }

  auto written = file.write(buf);
  assert(written == data.size() * sizeof(Record));
}

template <typename T>
typename FixedTable<T>::BidirIterator FixedTable<T>::begin() { return BidirIterator(*this); }

template <typename T>
typename FixedTable<T>::BidirIterator FixedTable<T>::end() {
  return BidirIterator(*this, file.size());
}


//FixedTable::BidirIterator implementation
template <typename T>
FixedTable<T>::BidirIterator::BidirIterator(FixedTable<T> &ft) : BidirIterator(ft, 0) {}

template <typename T>
FixedTable<T>::BidirIterator::BidirIterator(FixedTable<T> &ft, file::offset_t offset)
    : ft_(ft), offset_(offset) {}

template <typename T>
FixedTable<T>::BidirIterator::BidirIterator(const FixedTable<T>::BidirIterator &it)
    : BidirIterator(it.ft, it.offset) {}

template <typename T>
bool FixedTable<T>::BidirIterator::operator==(const FixedTable<T>::BidirIterator &it) {
  return offset_ == it.offset_;
}

template <typename T>
bool FixedTable<T>::BidirIterator::operator<(const FixedTable<T>::BidirIterator &it) {
  return offset_ < it.offset_;
}

template <typename T>
bool FixedTable<T>::BidirIterator::operator>(const FixedTable<T>::BidirIterator &it) {
  return offset_ > it.offset_;
}

template <typename T>
typename FixedTable<T>::BidirIterator &FixedTable<T>::BidirIterator::operator++() {
  offset_ += serialize::size<Record>();
  return *this;
}

template <typename T>
typename FixedTable<T>::BidirIterator FixedTable<T>::BidirIterator::operator++(int) {
  BidirIterator iter(*this);
  offset_ += serialize::size<Record>();
  return iter;
}

template <typename T>
typename FixedTable<T>::BidirIterator &FixedTable<T>::BidirIterator::operator--() {
  offset_ -= serialize::size<Record>();
  return *this;
}

template <typename T>
typename FixedTable<T>::BidirIterator FixedTable<T>::BidirIterator::operator--(int) {
  BidirIterator iter(*this);
  offset_ -= serialize::size<Record>();
  return iter;
}

template <typename T>
T FixedTable<T>::BidirIterator::operator*() {
  return ft_.get(offset_);
}

}
}
