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


#include <stdio.h>

#include <dirent.h>
#include <iostream>
#include "block_store_flat.h"
#include "flat_iterator.h"
#include <experimental/filesystem>


namespace fs = std::experimental::filesystem;

namespace block_store {

void BlockStoreFlat::add(uint32_t id, const std::vector<uint8_t> &block) {
  auto next_id = id;
  std::string file_name = dump_dir + "/" + id_to_name(next_id);
  // Write block to binary file
  fs::path fh(file_name);
  if (!fs::exists(fh)) {
    // New file will be created
    FILE *pfile;
    pfile = fopen(file_name.c_str(), "wb");
    /*auto res = */fwrite(block.data(), sizeof(uint8_t), block.size(), pfile);
    fflush(pfile);
    fclose(pfile);

    // Update internals, release lock
    current_id = next_id;

  } else {
    // Already exists, something is wrong
    // Answer iroha status
    // TODO: handle this case
    std::cout << "File name already exist " << std::endl;
  }
}

BlockStoreFlat::~BlockStoreFlat() {}

BlockStoreFlat::BlockStoreFlat(const std::string &path) {
  // Check if path exist:
  fs::path fh(path);
  if (fs::exists(fh)) {
    if (fs::is_directory(path)) {
      // Directory exists
      dump_dir = path;
      current_id = check_consistency();

    } else {
      // We assume here that wrong path is not possible

    }
  } else {
    // New BlockStore
    if (!fs::create_directories(fh)) {
      // TODO: handle cannot create directory
      std::cout << "Error creating directory " << std::endl;
    }
    current_id = 0;
    dump_dir = path;
  }
}

uint32_t BlockStoreFlat::check_consistency() const {
  uint32_t tmp_id = 0u;
  if (!dump_dir.empty()) {
    fs::path dir(dump_dir);
    if (fs::is_directory(dir)) {
      // Directory iterator:
      struct dirent **namelist;
      auto status = scandir(dump_dir.c_str(), &namelist, NULL, alphasort);
      if (status < 0) {
        // TODO: handle internal error
      } else {
        uint n = status;
        tmp_id++;
        uint i = 1;
        while (++i < n) {
          if (id_to_name(tmp_id) != namelist[i]->d_name) {
            for (uint j = i; j < n; ++j) {
              fs::path file(dump_dir+"/"+namelist[j]->d_name);
              fs::remove(file);
            }
            break;
          }
          tmp_id = name_to_id(namelist[i]->d_name);
        }

        for (uint j = 0; j < n; ++j) {
          free(namelist[j]);
        }
        free(namelist);
      }

    } else {
      // Not a directory
      // TODO: handle not a directory
      std::cout << "Not a directory " << std::endl;
    }
  }
  return tmp_id;
}

std::vector<uint8_t> BlockStoreFlat::get(uint32_t id) const {
  std::string filename = dump_dir + "/" + id_to_name(id);
  fs::path fh(filename);
  if (fs::exists(fh)) {
    auto f_size = fs::file_size(fh);
    std::vector<uint8_t> buf(f_size);
    FILE *pfile = fopen(filename.c_str(), "rb");
    fread(&buf[0], sizeof(uint8_t), f_size, pfile);
    fclose(pfile);
    return buf;

  } else {
    std::cout << "No file with this id found" << std::endl;
    // TODO: handle not found
    return std::vector<uint8_t>();
  }
}


std::string BlockStoreFlat::id_to_name(uint64_t id) const {
  std::string new_id(16, '\0');
  sprintf(&new_id[0], "%016lu", id);
  return new_id;
}

uint32_t block_store::BlockStoreFlat::last_id() const { return current_id; }

uint64_t BlockStoreFlat::name_to_id(std::string name) const{
  std::string::size_type sz;
  return std::stoull(name, &sz);
}

BlockStore::Iterator BlockStoreFlat::begin() {
  BlockStore::Iterator iter(new FlatIterator(*this, 1));
  return iter;
}
BlockStore::Iterator BlockStoreFlat::end() {
  BlockStore::Iterator iter(new FlatIterator(*this, last_id() + 1));
  return iter;
}
void BlockStoreFlat::remove(uint32_t id) {
  // Assume that id exists
  fs::path file(dump_dir+"/"+id_to_name(id));
  fs::remove(file);
}

// Flat Iterator realization

BlockStoreFlat::FlatIterator::FlatIterator(BlockStoreFlat &bstore)
    : bstore_(bstore), n_(0) {}


BlockStoreFlat::FlatIterator::FlatIterator(BlockStoreFlat &bstore,
                                           uint32_t offset)
    : bstore_(bstore), n_(offset) {}

BlockStoreFlat::FlatIterator::FlatIterator(const FlatIterator &it)
    : bstore_(it.bstore_), n_(it.n_), value_(it.value_) {}


bool BlockStoreFlat::FlatIterator::operator==(
    const block_store::BlockStore::AbstractIterator &it) {
  return n_ == dynamic_cast<const FlatIterator *>(&it)->n_;
}

bool BlockStoreFlat::FlatIterator::operator!=(
    const BlockStore::AbstractIterator &it) {
  return n_ != dynamic_cast<const FlatIterator *>(&it)->n_;
}


BlockStore::AbstractIterator &BlockStoreFlat::FlatIterator::operator=(
    const BlockStore::AbstractIterator &it) {
  return *this;
}
bool BlockStoreFlat::FlatIterator::operator<(
    const BlockStore::AbstractIterator &it) {
  return n_ < dynamic_cast<const FlatIterator *>(&it)->n_;
}
bool BlockStoreFlat::FlatIterator::operator>(
    const BlockStore::AbstractIterator &it) {
  return n_ > dynamic_cast<const FlatIterator *>(&it)->n_;
}
bool BlockStoreFlat::FlatIterator::operator>=(
    const BlockStore::AbstractIterator &it) {
  return n_ >= dynamic_cast<const FlatIterator *>(&it)->n_;
}
bool BlockStoreFlat::FlatIterator::operator<=(
    const BlockStore::AbstractIterator &it) {
  return n_ <= dynamic_cast<const FlatIterator *>(&it)->n_;
}
const std::vector<uint8_t> &BlockStoreFlat::FlatIterator::operator*() {
  value_ = bstore_.get(n_);
  return value_;
}
BlockStore::AbstractIterator &BlockStoreFlat::FlatIterator::operator++() {
  n_ += 1;
  return *this;
}

BlockStore::AbstractIterator &BlockStoreFlat::FlatIterator::operator--() {
  n_ -= 1;
  return *this;
}

BlockStore::AbstractIterator &BlockStoreFlat::FlatIterator::operator+=(
    const int &n) {
  n_ += n;
  return *this;
}
BlockStore::AbstractIterator &BlockStoreFlat::FlatIterator::operator-=(
    const int &n) {
  n_ -= n;
  return *this;
}
BlockStore::AbstractIterator &BlockStoreFlat::FlatIterator::operator-(
    const int &n) {
  n_ -= n;
  return *this;
}
BlockStore::AbstractIterator &BlockStoreFlat::FlatIterator::operator+(
    const int &n) {
  n_ += n;
  return *this;
}
}
