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

File::~File() {}

AppendableFile::AppendableFile(const std::string &path) : File(path) {
  file_.reset(fopen(path_.c_str(), "ab"));
}

template <>
void AppendableFile::append<ByteArray>(const ByteArray &data) {
  fwrite(data.data(), sizeof(ByteArray::value_type), data.size(), file_.get());
}

SequentialFile::SequentialFile(const std::string &path) : File(path) {
  file_.reset(fopen(path_.c_str(), "rb"));
}

template <>
void SequentialFile::read<ByteArray::value_type>(ByteArray::value_type *data,
                                                 std::size_t size,
                                                 std::size_t offset) {
  fseek(file_.get(), offset, SEEK_CUR);
  fread(data, sizeof(ByteArray::value_type), size, file_.get());
}


ByteArray SequentialFile::read(std::size_t size, std::size_t offset) {
  ByteArray ret(size);
  fseek(file_.get(), offset, SEEK_CUR);
  fread(ret.data(), sizeof(ByteArray::value_type), size, file_.get());
  // http://stackoverflow.com/a/17473871/1953079
  return ret;
}

}  // namespace file
}  // namespace ametsuchi
