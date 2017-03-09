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

#ifndef AMETSUCHI_RW_FILE_PLAIN_H
#define AMETSUCHI_RW_FILE_PLAIN_H

#include <ametsuchi/file/rw_file.h>
#include <string>

namespace ametsuchi {
namespace file {

/**
 * RWFilePlain does:
 *  - read and write files
 *
 * RWFilePlain does not:
 *  - create system file locks
 *  - use write-ahead log
 */
class RWFilePlain : public ReadWriteFile {
 public:
  explicit RWFilePlain(const std::string &path);

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
};
}
}

#endif  // AMETSUCHI_RW_FILE_PLAIN_H
