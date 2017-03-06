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

#include <ametsuchi/file/file.h>
#include <ametsuchi/globals.h>

namespace ametsuchi {
namespace file {

File::File(const std::string &path)
    : path_(path), file_(nullptr, &std::fclose) {}

offset_t File::position() const {
  return std::ftell(file_.get());
}

File::~File() {}
void File::close() {
  file_.reset(nullptr);
}

ByteArray SequentialFile::read(std::size_t size, offset_t offset) {
  ByteArray ret(size);
  std::fseek(file_.get(), offset, SEEK_CUR);
  auto res = std::fread(ret.data(), sizeof(ByteArray::value_type), size, file_.get());
  ret.resize(res);
  // http://stackoverflow.com/a/17473871/1953079
  return ret;
}

bool SequentialFile::open() {
  file_.reset(std::fopen(path_.c_str(), "rb"));
  return !!file_;
}

bool AppendableFile::open() {
  file_.reset(std::fopen(path_.c_str(), "ab"));
  return !!file_;
}
}  // namespace file
}  // namespace ametsuchi
