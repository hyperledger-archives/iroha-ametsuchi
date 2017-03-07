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

#include <ametsuchi/exception.h>

namespace ametsuchi {
namespace file {

/////////
/// File
File::File(const std::string &path)
    : path_(path), file_(nullptr, &std::fclose), read_(false), write_(false) {}

File::~File() {}

offset_t File::position() const { return std::ftell(file_); }

size_t File::size() const { return size_; }

bool File::is_open() { return file_ != nullptr; }

bool File::can_read() { return read_; }

bool File::can_write() { return write_; }

void File::close() { file_.reset(nullptr); }

void File::seek_from_current(offset_t offset) {
  std::fseek(file_, offset, SEEK_CUR);
}

void File::seek_from_start(offset_t offset) {
  std::fseek(file_, offset, SEEK_SET);
}

ByteArray File::read(size_t size) {
  ByteArray ret(size);
  auto res = std::fread(ret.data(), sizeof(ametsuchi::byte_t), size, file_);

  if (res != size) ret.resize(res);

  return ret;
}

////////////////
/// ReadOnlyFile
ReadOnlyFile::ReadOnlyFile(const std::string &path)
    : File(path), read_(true), write_(false) {}




/////////////////
/// ReadWriteFile
ReadWriteFile::ReadWriteFile(const std::string &path)
    : File(path), read_(true), write_(true) {}


offset_t ReadWriteFile::append(const ByteArray &data) {
  seek_to_end();

  size_t old_fsize = size_;
  size_t size      = data.size();

  size_ += size;
  if (write(data) != size) {
    return -1;
  }

  return old_fsize;  // offset at which data is written
}


size_t ReadWriteFile::write(const ByteArray &data) {
  auto res =
      std::fwrite(data.data(), sizeof(ametsuchi::byte_t), data.size(), file_);
  std::fflush(file_);
  return res;
}


}  // namespace file
}  // namespace ametsuchi
