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
#include "iterator.h"

namespace ametsuchi {
namespace table {

using flag_t = uint8_t;

class Field {
 public:
  class ForwardIterator;

  struct Record;

  enum class Flag : flag_t {
    VALID   = 0,
    REMOVED = 1
    // rest is reserved for future use
  };

  explicit Field(const std::string &path);
  ~Field();


  /**
   * Appends value in the end in the following format:
   *   flag | length | bytes
   *  ubyte | ulong  | ubyte[length]
   *
   * @param value
   * @return offset to this value in file
   */
  offset_t append(const ByteArray &value);

  /**
   * Reads value in table by given offset
   * @return ByteArray
   */
  ByteArray get(const offset_t offset);

  /**
   * Updates value at the given offset. Algorithm is the following:
   *   - append new value in the end
   *   - invalidate old value (change flag to file::Flag::REMOVED)
   *   - seek to end
   * @param offset
   * @param new_value
   * @return
   */
  bool update(const offset_t offset, const ByteArray &new_value);

  /**
   * Removes record from table.
   *    - seek to offset
   *    - overwrite flag
   *    - seek to end
   * @param offset
   * @return
   */
  bool remove(const offset_t offset);

  ForwardIterator begin();
  ForwardIterator end();

 private:
  file::ReadWriteFile rw_;
  std::string         path_;
  offset_t            file_size;
};


class Field::ForwardIterator : public Iterator {
 public:
  ForwardIterator(Field &ft);
  ForwardIterator(Field &ft, offset_t offset);
  ForwardIterator(const ForwardIterator &it);
  ForwardIterator &operator++();               // postfix++
  ForwardIterator operator++(int);             // ++prefix
  ByteArray &operator*();                      // dereference
  bool operator==(const ForwardIterator &it);  // ==
  bool operator<(const ForwardIterator &it);   // <
  bool operator>(const ForwardIterator &it);   // >
 protected:
  Field &   ft_;
  offset_t  offset_;
  ByteArray value_;
};


struct Field::Record {
  Field::Flag flag;
  ByteArray   data;
};

}  // namespace table
}  // namespace ametsuchi

#endif  // AMETSUCHI_FIELD_TABLE_H
