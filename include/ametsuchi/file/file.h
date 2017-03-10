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

#ifndef AMETSUCHI_FILE_H
#define AMETSUCHI_FILE_H

#include <ametsuchi/globals.h>
#include <spdlog/spdlog.h>
#include <sys/stat.h>   //
#include <sys/types.h>  // for stat, access
#include <unistd.h>     //
#include <memory>
#include <string>

namespace ametsuchi {
namespace file {

using flag_t = uint8_t;
using offset_t = int64_t;

#define PACKED_STRUCT __attribute__ ((__packed__))

#define INVALID_OFFSET INT64_MAX;

inline bool is_valid(offset_t offset) {
  return offset != INVALID_OFFSET;
}

/**
 * Abstraction over file operations:
 *  - open
 *  - read
 *  - write
 *  - close
 *  - remove
 *  - seek
 */
class File {
 public:
  explicit File(const std::string &path);
  virtual ~File() = 0;

  virtual bool open();
  void close();

  bool is_opened();

  /**
   * Optimized seek. If offset is closer to cursor position, seek will be
   * performed from current pointer position, otherwise from start
   * @param offset
   */
  void seek(offset_t offset);
  void seek_to_end();
  void seek_to_start();

  offset_t position() const;
  size_t size() const;

  bool can_read();
  bool can_write();

  std::string get_name();

  /**
   * Remove file.
   * @return true if successfully, false otherwise
   */
  bool remove();

  /**
   * Clears contents of file.
   * @return true, if successfully, false otherwise
   */
  bool clear();

  /**
   * No need for file being opened.
   * @return true if file exists.
   */
  bool exists() const;

  /**
   * Reads exactly \p size bytes from file at current cursor position.
   * @param size
   * @return byte array of \p size
   */
  ByteArray read(size_t size);

  const std::string get_path() const;

 protected:
  bool read_;   // true if I can read
  bool write_;  // true if I can write

  size_t size_;  // current size of file in bytes

  std::string path_;  // path to current file
  std::unique_ptr<FILE, decltype(&std::fclose)> file_;

 private:
  void seek_from_start(offset_t offset);
  void seek_from_current(offset_t offset);
};


/**
 * ROFile is used to concurrently open for read as many files as you wish.
 *  - no locking
 *  - individual cursor for each ROFile
 */
class ROFile : public File {
 public:
  explicit ROFile(const std::string &path);
  bool open() override;
};


}  // namespace file
}  // namespace ametsuchi

#endif  // AMETSUCHI_FILE_H
