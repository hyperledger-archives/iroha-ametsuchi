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

#ifndef AMETSUCHI_INDEX_H
#define AMETSUCHI_INDEX_H

#include <string>
#include <ametsuchi/file/file.h>
#include <ametsuchi/file/rw_file.h>

namespace ametsuchi {
namespace tx_store {

/**
 * Stores an array of offset_t, append-only
 */
class Index {
 public:
  /**
   * Open (create if not exists) index in given path
   * @param path index file path
   */
  Index(const std::string &path);

  /**
   * Get nth element
   * @param n requested element index
   * @return offset of element
   */
  file::offset_t get(std::size_t n);

  /**
   * Append new offset to index
   * @param offset given offset to append
   * @return index of appended element (size() - 1 ?)
   */
  std::size_t append(const file::offset_t offset);

  /**
   * Size of the index
   * @return number of elements in the index
   */
  std::size_t size() const;

  // TODO removal?

 private:
  file::RWFile file_;
};

}  // namespace tx_store
}  // namespace ametsuchi

#endif //AMETSUCHI_INDEX_H
