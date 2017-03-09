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

namespace ametsuchi {
namespace file {

static auto console = spdlog::stdout_color_mt("file");

/////////
/// File
File::File(const std::string &path)
    : read_(false), write_(false), path_(path), file_(nullptr, &std::fclose) {}

File::~File() {}

bool File::open() {
  struct stat statistics;  // https://linux.die.net/man/2/stat
  if (stat(path_.c_str(), &statistics) == -1) {
    console->debug("{} does not exist", path_);
    return false;
  }
  size_ = (size_t)(statistics.st_size);
  console->debug("{} is opened, its size {}", path_, size_);
  return true;
}

offset_t File::position() const { return std::ftell(file_.get()); }

size_t File::size() const { return size_; }

bool File::is_opened() { return file_.get() != nullptr; }

bool File::can_read() { return read_; }

bool File::can_write() { return write_; }

void File::close() {
  file_.reset(nullptr);
  size_ = 0;
}

void File::seek_from_current(offset_t offset) {
  std::fseek(file_.get(), offset, SEEK_CUR);
}

void File::seek_from_start(offset_t offset) {
  std::fseek(file_.get(), offset, SEEK_SET);
}

void File::seek_to_end() { std::fseek(file_.get(), 0, SEEK_END); }

void File::seek_to_start() { rewind(file_.get()); }

ByteArray File::read(size_t size) {
  ByteArray buf(size, 0);
  auto res = std::fread(&buf[0], sizeof(ametsuchi::byte_t), size, file_.get());
  return buf;
}

const std::string File::get_path() const { return path_; }

bool File::remove() {
  close();
  return 0 == unlink(path_.c_str());
}


void File::seek(offset_t offset) {
  offset_t pos = position();
  if (offset > pos) {
    seek_from_current(offset - pos);
    // } else if (offset < pos && pos - offset > offset) {
    //   seek_from_current(pos - offset);
  } else {
    seek_from_start(offset);
  }
}


bool File::clear() {
  auto deleted = remove();
  auto opened = open();
  return deleted && opened;
}


bool File::exists() const { return access(path_.c_str(), F_OK) != -1; }

void File::set_path(const std::string &path) {
  if (is_opened()) {
    close();
  }
  path_ = path;
}


std::string File::get_name() {
  if (fname_ == "") {
    // parse name from path.
    // name = everything after first slash in path
    std::string name = "";
    while (*path_.rbegin() != '/') {
      name.push_back(*path_.rbegin());
    }
    std::reverse(name.begin(), name.end());
    fname_ = name;
  }

  return fname_;
}


////////////////
/// ReadOnlyFile
ReadOnlyFile::ReadOnlyFile(const std::string &path) : File::File(path) {
  read_ = true;
  write_ = false;
}


bool ReadOnlyFile::open() {
  // to read statistics
  bool opened = File::open();

  file_.reset(std::fopen(path_.c_str(), "rb"));
  return !!file_ && opened;
}


}  // namespace file
}  // namespace ametsuchi
