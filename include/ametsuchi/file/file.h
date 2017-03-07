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
#include <memory>
#include <string>

namespace ametsuchi {
namespace file {


using flag_t = uint8_t;


class File {
 public:
  explicit File(const std::string &path);
  virtual ~File() = 0;

  virtual bool open() = 0;
  void         close();

  bool is_open();

  void seek_from_start(offset_t offset);
  void seek_from_current(offset_t offset);

  void seek_to_end();
  void seek_to_start();

  offset_t position() const;
  size_t   size() const;

  bool can_read();
  bool can_write();

  /**
   * Reads exactly \p size bytes from file at current cursor position.
   * @param size
   * @return byte array of \p size
   */
  ByteArray read(size_t size);

 protected:
  bool read_;   // true if I can read
  bool write_;  // true if I can write

  size_t size_;  // current size of file in bytes

  std::string path_;  // path to current file
  std::unique_ptr<FILE, decltype(&std::fclose)> file_;
};


/**
 * ReadWriteFile is used to concurrently open as many files as you wish.
 * No locking, individual pointer.
 */
class ReadOnlyFile : public File {
  using File::File;

 public:
  explicit ReadOnlyFile(const std::string &path);
  bool open() override;
};


/**
 * ReadWriteFile is used to write to single file. Installs lock on this file.
 * Only one writer to file is possible.
 */
class ReadWriteFile : public File {
  using File::File;
  // TODO(warchant): add file locking after open using flock or smth like this
 public:
  explicit ReadWriteFile(const std::string &path);
  bool open() override;

  /**
   * Appends \p data to the end of file.
   * @param data
   * @return offset, at which data is appended; -1 if not all data is appended
   */
  offset_t append(const ByteArray &data);

  /**
   * Writes \p data at current position.
   * @param data
   * @return number of written bytes
   */
  size_t write(const ByteArray &data);
};


}  // namespace file
}  // namespace ametsuchi

#endif  // AMETSUCHI_FILE_H
