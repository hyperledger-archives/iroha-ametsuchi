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
#include <ametsuchi/serializer.h>
#include <cassert>

namespace ametsuchi {
namespace file {

RWFileSafe::RWFileSafe(const std::string &path) : ReadWriteFile(nullptr) {
  file_->
}

bool RWFileSafe::open() {
  if (wal_->exists()) {
    // then crash happened and we need recovery
    recover();
  }
  return file_->open();
}

offset_t RWFileSafe::append(const ByteArray &data) {
  seek_to_end();

  size_t old_fsize = size_;
  size_t size = data.size();

  size_ += size;
  size_t written;
  if ((written = this->write(data)) != size) {
    console->critical("we write {} bytes, but {} written", size, written);
    throw exception::IOError("RWFileSafe::append");
  }

  return old_fsize;  // offset at which data is written
}

/**
 * 1. Write to LOG:
 *   | cursor position | data size | data             |
 *   | ulong           | ulong     | ubyte[data size] |
 * 2. Write data to FILE
 * 3. Remove file with LOG
 */
size_t RWFileSafe::write(const ByteArray &data) {
  // ensure LOG does not exist
  assert(!wal_->exists());

  // write into log and then into file
  ByteArray buf(2 * sizeof(uint64_t) + data.size());
  byte_t *ptr = buf.data();

  serialize::put(ptr, file_->position());
  serialize::put(ptr, data.size());
  serialize::put(ptr, data);

  if (wal_->open()) {
    wal_->write(buf);
  } else {
    throw exception::IOError("can not open LOG: " + wal_->get_path());
  }

  file_->write(data);

  wal_->remove();
}

void RWFileSafe::recover() {
  assert(!file_->is_opened());
  assert(!wal_->is_opened());
  assert(wal_->exists());

  if (!wal_->open()) {
    throw exception::IOError("can not open LOG: " + wal_->get_path());
  }

  // file size before write AND length of data
  ByteArray buf = wal_->read(2 * sizeof(uint64_t));
  const byte_t *ptr = buf.data();

  uint64_t position, length;
  serialize::get(&position, ptr);
  serialize::get(&length, ptr);

  ByteArray data = wal_->read(length);

  file_->seek(position);
  file_->write(data);

  wal_->remove();
}

}  // namespace file
}  // namespace ametsuchi
