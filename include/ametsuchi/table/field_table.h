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

#include <ametsuchi/file/file.h>
#include <ametsuchi/globals.h>
#include <memory>
#include <string>

namespace ametsuchi {
namespace table {

using flag_t = uint8_t;

class Field {
 public:
  class ForwardIterator;

  struct Record;

  explicit Field(const std::string &path);
  ~Field();


  /**
   * Appends value in the end in the following format:
   *   flag | length | bytes
   *  ubyte | ulong  | ubyte[length]
   *
   * @param data
   * @return offset to this value in file
   */
  offset_t append(const ByteArray &data);

  /**
   * Reads value in table by given offset
   * @return ByteArray of zero length if record is removed or in case of error
   * or data
   */
  ByteArray get(const offset_t offset);

  /**
   * Updates value at the given offset. Algorithm is the following:
   *    - invalidate old value (change flag to Table::Flag::REMOVED)
   *    - seek in the end
   *    - append new value in the end
   * @param offset
   * @param new_value
   * @return
   */
  bool update(const offset_t offset, const ByteArray &new_value);

  /**
   * Removes record from table.
   *    - seek to offset
   *    - overwrite flag
   * @param offset
   * @return
   */
  bool remove(const offset_t offset);


  /**
   * After updates or removes invalidated memory will be
   */
  void compact();

  /**
   * Returns iterator to begin of Field
   * @return
   */
  ForwardIterator begin();

  /**
   * Returns iterator to end of Field
   * @return
   */
  ForwardIterator end();

  /**
   * Getter for file path.
   * @return
   */
  std::string path();

 private:
  file::ReadWriteFile f_;
  std::string         path_;
  offset_t            file_size;
};


class Field::ForwardIterator {
 public:
  ForwardIterator(Field &field);
  ForwardIterator(Field &field, offset_t offset);
  ForwardIterator(const ForwardIterator &it);
  bool operator==(const ForwardIterator &it);
  bool operator<(const ForwardIterator &it);
  bool operator>(const ForwardIterator &it);
  ByteArray &operator*();
  ForwardIterator &operator++();    // postfix++
  ForwardIterator operator++(int);  // ++prefix
 protected:
  Field &   ft_;
  offset_t  offset_;
  ByteArray value_;
};


/**
 * Record is private data structure
 */
struct Field::Record {
  enum Flag : flag_t {
    REMOVED = 1,
    VALID   = 2
    // rest is reserved for future use
  };

  flag_t    flag;
  ByteArray data;
};

}  // namespace table
}  // namespace ametsuchi

#endif  // AMETSUCHI_FIELD_TABLE_H
