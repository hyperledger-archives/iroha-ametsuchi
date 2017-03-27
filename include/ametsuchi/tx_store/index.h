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

#include <ametsuchi/cache.h>
#include <ametsuchi/file/file.h>
#include <ametsuchi/file/rw_file.h>
#include <string>

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
   * **
   * Open (create if not exists) index in given path
   * @param path index file path
   * @param inMemSize Max size of index to store in-memory
   */
  Index(const std::string &path, const std::size_t inMemSize);


  /**
   * Get nth element
   * @param n requested element index
   * @return offset of element
   */
  file::offset_t get(std::size_t n);

  /**
   * Get last element in file
   * @param n requested element index
   * @return offset of element
   */
  file::offset_t get_last();

  /**
   * Append new offset to index
   * @param offset given offset to append
   * @return index of appended element (size() - 1 ?)
   */
  std::size_t append(file::offset_t offset);

  /**
   * Appends batch of offsets atomically
   * @param offsets
   * @return index of the first appended element
   */
  std::size_t append_batch(std::vector<file::offset_t> &offsets);


  /**
  * Commit submitted changes, before commit written data is not accessible
  */
  void commit();

  /**
   * Undo uncommited changes
   */
  void rollback();

  /**
   * @return true if there are no uncommitted changes and false otherwise
   */
  bool is_committed() const;


  /**
   * Size of the index
   * @return number of elements in the index
   */
  std::size_t size() const;

  /**
   * Set size for in_mem cache
   * @param cache_size
   */
  void set_cache_size(std::size_t cache_size);


 private:
  file::RWFile file_;
  std::vector<file::offset_t> uncommitted_; // vector of uncommitted changes
  file::offset_t uncommitted_size_; // size (in bytes) of uncommited changes (don't confuse with uncommited_.size() )

  file::offset_t last_;
  std::size_t last_index_;

  Cache<std::size_t, file::offset_t> cache;
  std::vector<file::offset_t> inMemData;
};

}  // namespace tx_store
}  // namespace ametsuchi

#endif  // AMETSUCHI_INDEX_H
