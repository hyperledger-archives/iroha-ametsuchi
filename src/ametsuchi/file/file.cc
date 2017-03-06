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
#include <unistd.h>

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

template <>
std::size_t AppendableFile::append<ByteArray>(const ByteArray &data) {
  auto res = std::fwrite(data.data(), sizeof(ByteArray::value_type), data.size(), file_.get());
  std::fflush(file_.get());
  return res;
}

template <>
std::size_t SequentialFile::read<ByteArray::value_type>(ByteArray::value_type *data,
                                                 std::size_t size,
                                                 offset_t offset) {
  std::fseek(file_.get(), offset, SEEK_CUR);
  return std::fread(data, sizeof(ByteArray::value_type), size, file_.get());
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

bool RandomAccessFile::open() {
  file_.reset(std::fopen(path_.c_str(), "rb"));
  fd_ = fileno(file_.get());
  return !!file_;
}

template <>
std::size_t RandomAccessFile::read<ByteArray::value_type>(ByteArray::value_type *data,
                                                        std::size_t size,
                                                        offset_t offset) {
  auto res = pread(fd_, data, size * sizeof(ByteArray::value_type), offset);
  return res < 0 ? 0 : res;
}

ByteArray RandomAccessFile::read(std::size_t size, offset_t offset) {
  ByteArray ret(size);
  auto res = pread(fd_, ret.data(), size * sizeof(ByteArray::value_type), offset);
  if (res < 0){
    res = 0;
  }
  ret.resize(res);
  return ret;
}

bool MMapFile::open() {
  file_.reset(std::fopen(path_.c_str(), "r+b"));
  fd_ = fileno(file_.get());
  return !!file_;
}

void MMapFile::close() {
  File::close();
//  munmap();
}
}  // namespace file
}  // namespace ametsuchi