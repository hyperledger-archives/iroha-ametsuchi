/**
 * Copyright Soramitsu Co., Ltd. 2017 All Rights Reserved.
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

#include <ametsuchi/file/rw_file.h>
#include <spdlog/spdlog.h>

namespace ametsuchi {
namespace file {

static auto console = spdlog::stdout_color_mt("rw_file");

RWFile::RWFile(const std::string &path) : File(path) {
  read_ = true;
  write_ = true;
}

bool RWFile::open() {
  file_.reset(std::fopen(path_.c_str(), "r+b"));
  if (!file_.get()) {
    file_.reset(std::fopen(path_.c_str(), "w+b"));
  }
  // to read statistics
  bool opened = File::open();
  return !!file_ && opened;
}


offset_t RWFile::append(const ByteArray &data) {
  seek_to_end();

  size_t old_fsize = size_;
  size_t size = data.size();

  size_t written;
  if ((written = write(data)) != size) {
    console->critical("we write {} bytes, but {} written", size, written);
    throw exception::IOError("RWFilePlain::append");
  }

  return old_fsize;  // offset at which data is written
}

offset_t RWFile::append(const byte_t *ptr, size_t n) {
  seek_to_end();

  size_t old_fsize = size_;
  size_t size = n;

  size_t written;
  if ((written = std::fwrite(ptr, sizeof(byte_t), n, file_.get())) != size) {
    console->critical("we write {} bytes, but {} written", size, written);
    throw exception::IOError("RWFilePlain::append");
  }
  std::fflush(file_.get());
  size_ += written;
  return old_fsize;  // offset at which data is written
}

size_t RWFile::write(const ByteArray &data) {
  auto res = std::fwrite(data.data(), sizeof(ametsuchi::byte_t), data.size(),
                         file_.get());
  std::fflush(file_.get());
  size_ += res;
  return res;
}

RWFile::~RWFile() {}
}
}
