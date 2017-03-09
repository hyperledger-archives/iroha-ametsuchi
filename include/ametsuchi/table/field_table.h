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

#ifndef AMETSUCHI_FIELD_TABLE_H
#define AMETSUCHI_FIELD_TABLE_H

#include <ametsuchi/file/rw_file_safe.h>
#include <ametsuchi/globals.h>
#include <ametsuchi/table/table.h>
#include <memory>
#include <string>

namespace ametsuchi {
namespace table {

class FieldTable {
 public:
  class ForwardIterator;

  using Record = BaseRecord<ByteArray>;

  FieldTable(const std::string &path);
  ~FieldTable();


  /**
   * Appends value in the end in the following format:
   *   flag | length | bytes
   *  ubyte | ulong  | ubyte[length]
   *
   * @param data
   * @return offset to this value in file
   */
  file::offset_t append(const ByteArray &data);

  /**
   * Reads value in table by given offset
   * @return ByteArray of zero length if record is removed or in case of error
   * or data
   */
  ByteArray get(const file::offset_t offset);

  /**
   * Updates value at the given offset. Algorithm is the following:
   *    - invalidate old value (change flag to Table::Flag::REMOVED)
   *    - seek in the end
   *    - append new value in the end
   * @param offset
   * @param new_value
   * @return
   */
  file::offset_t update(const file::offset_t offset,
                        const ByteArray &new_value);

  /**
   * Removes record from table.
   *    - seek to offset
   *    - overwrite flag
   * @param offset
   * @return
   */
  bool remove(const file::offset_t offset);


  /**
   * After updates or removes invalidated memory will be
   */
  void compact();

  /**
   * Returns iterator to begin of FieldTable
   * @return
   */
  ForwardIterator begin();

  /**
   * Returns iterator to end of FieldTable
   * @return
   */
  ForwardIterator end();

  /**
   * Getter for file path.
   * @return
   */
  std::string path();

 private:
  std::unique_ptr<file::ReadWriteFile> f_;
  file::offset_t file_size;
};


class FieldTable::ForwardIterator {
 public:
  ForwardIterator(FieldTable &field);
  ForwardIterator(FieldTable &field, file::offset_t offset);
  ForwardIterator(const ForwardIterator &it);
  bool operator==(const ForwardIterator &it);
  bool operator<(const ForwardIterator &it);
  bool operator>(const ForwardIterator &it);
  ByteArray &operator*();
  ForwardIterator &operator++();    // postfix++
  ForwardIterator operator++(int);  // ++prefix
 protected:
  FieldTable &ft_;
  file::offset_t offset_;
  ByteArray value_;
};


}  // namespace table
}  // namespace ametsuchi

#endif  // AMETSUCHI_FIELD_TABLE_H
