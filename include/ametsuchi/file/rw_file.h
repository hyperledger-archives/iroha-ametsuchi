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

#ifndef AMETSUCHI_READ_WRITE_FILE_H_
#define AMETSUCHI_READ_WRITE_FILE_H_

#include <ametsuchi/file/file.h>
#include <string>

namespace ametsuchi {
namespace file {

/**
 * ReadWriteFile is an interface.
 *
 * ReadWriteFile does:
 *  - open file for read+write permissions
 *
 * ReadWriteFiles does not:
 *  - know how to write to file
 */
class RWFile : public File {
 public:
  explicit RWFile(const std::string &path);

  /**
   * Opens file. Creates file if not exist.
   * @return true if file is opened without errors
   */
  bool open() override;

  /**
   * Appends \p data to the end of file.
   * @param data
   * @return offset, at which data is appended or empty byte array
   */
  virtual offset_t append(const ByteArray &data);

  virtual offset_t append(const byte_t* ptr, size_t n);

  /**
   * Writes \p data at current position.
   * @param data
   * @return number of written bytes
   */
  virtual size_t write(const ByteArray &data);

  /**
   * Virtual destructor. It is needed to make sure that derived classes call
   * correct destructor.
   */
  virtual ~RWFile();
};

}  // namespace file
}  // namespace ametsuchi

#endif  // AMETSUCHI_READ_WRITE_FILE_H_
