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

#ifndef AMETSUCHI_ARRAY_H
#define AMETSUCHI_ARRAY_H

#include <string>
#include <ametsuchi/file/file.h>
#include <ametsuchi/file/rw_file.h>
#include <ametsuchi/tx_store/index.h>

namespace ametsuchi {
namespace tx_store {

/**
 * Stores array of ByteArrays, append-only
 */
class Array {
 public:

  class RandomAccessIterator;



  /**
   * Open (create if not exists) array in given path
   * @param path array file path
   */
  Array(const std::string &path);

  /**
   * Append new ByteArray to array
   * @param data ByteArray to append
   * @return offset of appended ByteArray
   */
  std::size_t append(const ByteArray &data);
  std::size_t crash_append(const ByteArray &data);

  /**
   * Get ByteArray by offset
   * @param offset offset of ByteArray
   * @return requested ByteArray
   */
  ByteArray get(const std::size_t n);

  RandomAccessIterator begin();

  RandomAccessIterator end();

 private:
  // TODO separate write/read logic?
  file::RWFile file_;
  Index index_;

};

}  // namespace tx_store
}  // namespace ametsuchi

#endif //AMETSUCHI_ARRAY_H
