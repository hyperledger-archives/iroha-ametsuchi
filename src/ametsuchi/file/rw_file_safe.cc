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
#include <ametsuchi/serializer.h>
#include <cassert>

namespace ametsuchi {
namespace file {

static auto console = spdlog::stdout_color_mt("rw_file_safe");

RWFileSafe::RWFileSafe(const std::string &path)
    : RWFile(path),
      wal_(new RWFile(Env::get().get_logs_directory() + get_name() + ".log")) {}

bool RWFileSafe::open() {
  if (wal_->exists()) {
    // then crash happened and we need recovery
    recover();
  }
  return RWFile::open();
}

offset_t RWFileSafe::append(const ByteArray &data) {
  seek_to_end();

  size_t old_fsize = size_;
  size_t size = data.size();

  size_t written;
  if ((written = this->write(data)) != size) {
    console->critical("we write {} bytes, but {} written", size, written);
    throw exception::IOError("RWFileSafe::append");
  }

  return old_fsize;  // offset at which data is written
}

/**
 * 1. Write to LOG:
 *   //           header          \\
 *   | cursor position | data size | data             |
 *   | ulong           | ulong     | ubyte[data size] |
 * 2. Write data to FILE
 * 3. Remove LOG file
 */
size_t RWFileSafe::write(const ByteArray &data) {
  // ensure LOG does not exist
  assert(!wal_->exists());

  // write into log and then into file
  constexpr auto header_size = 2 * sizeof(uint64_t);
  ByteArray buf(header_size + data.size());
  byte_t *ptr = buf.data();

  serialize::put(ptr, position());
  serialize::put(ptr, data);

  if (wal_->open()) {
    size_t written = wal_->write(buf);
    if (written != buf.size()) {
      console->critical("writing to LOG {} bytes, but {} written", buf.size(),
                        written);
      throw exception::IOError("can not write to LOG");
    }
  } else {
    throw exception::IOError("can not open LOG: " + wal_->get_path());
  }

  size_t size = RWFile::write(data);
  if (size != data.size()) {
    recover();
  }

  // transaction completed
  wal_->remove();

  return size;
}


void RWFileSafe::recover() {
  console->debug("Attempting to recover using LOG: {}", wal_->get_path());

  assert(!this->is_opened());
  assert(!wal_->is_opened());
  assert(wal_->exists());

  if (!wal_->open()) {
    throw exception::IOError("can not open LOG: " + wal_->get_path());
  }

  size_t wal_size = wal_->size();

  // read whole LOG into memory
  ByteArray header = wal_->read(wal_size);
  const byte_t *ptr = header.data();

  // expected = position (8b) + size (8b)
  constexpr auto header_size = 2 * sizeof(uint64_t);
  if (header.size() < header_size) {
    console->debug(
        "write to log was interrupted, header.size()={}, should be {}",
        header.size(), header_size);
    wal_->remove();
  }

  uint64_t position, length;
  serialize::get(&position, ptr);
  serialize::get(&length, ptr);

  // next get reads length, so we need to move ptr back
  ptr -= sizeof(uint64_t);

  ByteArray data;
  serialize::get(&data, ptr);

  if (data.size() < length) {
    console->debug("write to log was interrupted, data.size()={}, should be {}",
                   data.size(), length);
    wal_->remove();
  }

  this->seek(position);
  RWFile::write(data);

  // transaction completed
  wal_->remove();
}


}  // namespace file
}  // namespace ametsuchi
