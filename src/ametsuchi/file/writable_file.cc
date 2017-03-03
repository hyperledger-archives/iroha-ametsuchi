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

#include <ametsuchi/file/writable_file.h>

namespace ametsuchi {
namespace file {

WritableFile::WritableFile(const std::string& path) : File(path) {}

bool WritableFile::open() {
  file = fopen(path.c_str(), "wb");
  if (!file)
    return false;
  else
    return true;
}



template <typename T>
void WritableFile::write(const T* blob, const uint64_t size) {
  throw std::exception("not implemented");
}

// for uint8_t
template <>
void WritableFile::write(const uint8_t* blob, const uint64_t size) {
  fwrite(blob, sizeof(uint8_t), size, _file);
}
}
}
