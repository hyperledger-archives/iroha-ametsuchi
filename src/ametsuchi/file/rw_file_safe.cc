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

#include <ametsuchi/file/rw_file_safe.h>
#include <ametsuchi/globals.h>
#include <cassert>

namespace ametsuchi {
namespace file {

RWFileSafe::RWFileSafe(const std::string &name) : ReadWriteFile(name) {

}

offset_t RWFileSafe::append(const ByteArray &data) {}

/**
 * 1. Write to LOG:
 *   | file size | data size | data
 *   | ulong     | ulong     | ubyte[data size]
 * 2. Write to FILE data
 * 3. Remove file with LOG
 */
size_t RWFileSafe::write(const ByteArray &data) {
  // ensure LOG does not exist
  assert(!wal_->exists());

  ByteArray buf(2 * sizeof(uint64_t) + data.size());
  byte_t *ptr = buf.data();

  wal_->write()
}
}
}
