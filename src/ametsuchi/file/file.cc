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

#include <ametsuchi/exception.h>
#include <ametsuchi/file/file.h>
#include <spdlog/spdlog.h>
#include <cstdio>

namespace ametsuchi {
namespace file {

static auto console = spdlog::stdout_color_mt("file");

/////////
/// File
File::File(const std::string &path)
    : path_(path), file_(nullptr, &std::fclose), read_(false), write_(false) {}

File::~File() {}

bool File::open() {
  if (stat(path_.c_str(), &statistics) == -1) {
    console->critical("can not get access to file " + path_);
    return false;
  }

  size_ = (size_t)(statistics.st_size);

  return true;
}

offset_t File::position() const { return std::ftell(file_.get()); }

size_t File::size() const { return size_; }

bool File::is_opened() { return file_.get() != nullptr; }

bool File::can_read() { return read_; }

bool File::can_write() { return write_; }

void File::close() { file_.reset(nullptr); }

void File::seek_from_current(offset_t offset) {
  std::fseek(file_.get(), offset, SEEK_CUR);
}

void File::seek_from_start(offset_t offset) {
  std::fseek(file_.get(), offset, SEEK_SET);
}

void File::seek_to_end() { std::fseek(file_.get(), 0, SEEK_END); }

void File::seek_to_start() { rewind(file_.get()); }

ByteArray File::read(size_t size) {
  ByteArray ret(size);
  auto      res =
      std::fread(ret.data(), sizeof(ametsuchi::byte_t), size, file_.get());

  if (res != size) ret.resize(res);

  return ret;
}

////////////////
/// ReadOnlyFile
ReadOnlyFile::ReadOnlyFile(const std::string &path) : File::File(path) {
  read_  = true;
  write_ = false;
}


bool ReadOnlyFile::open() {
  // to read statistics
  bool opened = File::open();

  file_.reset(std::fopen(path_.c_str(), "rb"));
  return !!file_ && opened;
}


/////////////////
/// ReadWriteFile
ReadWriteFile::ReadWriteFile(const std::string &path) : File::File(path) {
  read_  = true;
  write_ = true;
}


bool ReadWriteFile::open() {
  file_.reset(std::fopen(path_.c_str(), "r+b"));
  if (!file_.get()) {
    file_.reset(std::fopen(path_.c_str(), "w+b"));
  }
  // to read statistics
  bool opened = File::open();
  return !!file_ && opened;
}


offset_t ReadWriteFile::append(const ByteArray &data) {
  seek_to_end();

  size_t old_fsize = size_;
  size_t size      = data.size();

  size_ += size;
  size_t written;
  if ((written = write(data)) != size) {
    console->critical("we write " + std::to_string(size) + "bytes, but " +
                      std::to_string(written) + " written");
    throw exception::IOError("ReadWriteFile::append");
  }

  return old_fsize;  // offset at which data is written
}


size_t ReadWriteFile::write(const ByteArray &data) {
  auto res = std::fwrite(data.data(), sizeof(ametsuchi::byte_t), data.size(),
                         file_.get());
  std::fflush(file_.get());
  return res;
}


bool File::remove() {
  close();
  unlink(path_.c_str());
}

void File::seek(offset_t offset) {
  offset_t pos = position();
  if (offset > pos) {
    seek_from_current(offset - pos);
    //  } else if (offset < pos && pos - offset > offset) {
    //    seek_from_current(pos - offset);
  } else {
    seek_from_start(offset);
  }
}

}  // namespace file
}  // namespace ametsuchi
