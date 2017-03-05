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

#include <ametsuchi/file/file.h>

namespace ametsuchi {
namespace table {

Table::Table(const std::string &path) : path_(path), r_(path), w_(path) {}

bool Table::putBatch(
    const std::vector<std::pair<ByteArray, ByteArray>> &batch) {}

bool Table::put(const ByteArray &key, const ByteArray &value) {
  if (exists(key)) {
    // key exists, invalidate old key
    Record r;
  }
}

ByteArray &Table::get(const ByteArray &key) {}

bool Table::exists(const ByteArray &key) { return index_.exists(key); }

bool Table::remove(const ByteArray &key) {}

uint64_t Table::size() { return index_.size(); }

///////////////////////////
/// TABLE::ITERATOR IMPLEMENTATION
Table::Iterator::Iterator() : offset(0) {}
Table::Iterator::~Iterator() {}
Table::Iterator::Iterator(const Iterator &it);
Table::Iterator::Iterator(const Iterator &&it);
void Table::Iterator::operator=(const Iterator &r);    // =
void Table::Iterator::operator=(const Iterator &&r);   // =
Iterator &Table::Iterator::operator++();               // postfix++
Iterator &Table::Iterator::operator++(int);            // ++prefix
Iterator &Table::Iterator::operator--();               // postfix--
Iterator &Table::Iterator::operator--(int);            // --prefix
Record &Table::Iterator::operator*(){

}
Record &Table::Iterator::operator->();                 // dereference
bool Table::Iterator::operator==(const Iterator &it);  // ==
bool Table::Iterator::operator<(const Iterator &it);   // <
bool Table::Iterator::operator>(const Iterator &it);   // >

}  // namespace table
}  // namespace ametsuchi
