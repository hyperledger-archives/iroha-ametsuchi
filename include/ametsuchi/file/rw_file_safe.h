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

#ifndef AMETSUCHI_RW_FILE_SAFE_H
#define AMETSUCHI_RW_FILE_SAFE_H

#include <ametsuchi/file/rw_file_plain.h>
#include <string>

namespace ametsuchi {
namespace file {

/**
 * RWFileSafe does:
 *  - read and write files
 *  - create system file locks
 *  - use write-ahead log
 *  - performs crash recovery
 */
class RWFileSafe : public ReadWriteFile {
 public:
  explicit RWFileSafe(const std::string &path);

  bool open() override;

  /**
   * Appends \p data to the end of file.
   * @param data
   * @return offset, at which data is appended or empty byte array otherwise
   */
  offset_t append(const ByteArray &data) override;

  /**
   * Writes \p data at current position.
   * @param data
   * @return number of written bytes
   */
  size_t write(const ByteArray &data) override;

 private:
  // file_ masks File::file_
  // reason: we don't want to implement write() twice, in RWFilePlain and here
  std::unique_ptr<RWFilePlain> file_;
  std::unique_ptr<RWFilePlain> wal_;

  /**
   * Recovery mechanism.
   * file_ and wal_ should not be opened and wal_ should exist.
   */
  void recover();
};

}  // namespace file
}  // namespace ametsuchi

#endif  // AMETSUCHI_RW_FILE_SAFE_H
