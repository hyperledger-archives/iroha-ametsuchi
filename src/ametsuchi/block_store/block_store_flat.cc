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

#include <ametsuchi/block_store/block_store_flat.h>
#include <stdio.h>

#include <cppfs/FileHandle.h>
#include <cppfs/FileIterator.h>
#include <cppfs/fs.h>
#include <iostream>

namespace fs = cppfs::fs;
using cppfs::FileHandle;
using cppfs::FileIterator;

namespace ametsuchi {
namespace block_store {


std::string BlockStoreFlat::append(const std::vector<uint8_t> &block) {

  std::string next_id = get_next_id(current_id);
  std::string file_name = dump_dir + next_id + ".dmp";
  // Write block to binary file
  auto fh = fs::open(file_name);
  if (!fh.exists()) {
    // New file will be created
    FILE *pfile;
    pfile = fopen(file_name.c_str(), "wb");
    auto res = fwrite(block.data(), sizeof(uint8_t), block.size(), pfile);
    fflush(pfile);
    fclose(pfile);

    // Update internals, release lock ?
    current_id = next_id;
    return current_id;
  } else {
    // Already exists, something is wrong
    // Answer iroha status
    // TODO: handle this case
  }

}

BlockStoreFlat::~BlockStoreFlat() {}

BlockStoreFlat::BlockStoreFlat(const std::string &path) {
  // Check if path exist:
  FileHandle fh = fs::open(path);
  if (fh.exists()) {
    if (fh.isDirectory()) {
      // Directory exists
      dump_dir = path;
      current_id = get_last_id();
      if (current_id.empty()) {
        // TODO: check
      }


    } else {
      // Wrong path
      // TODO: handle wrong path
    }
  } else {
    // New BlockStore
    if (!fh.createDirectory()) {
      // TODO: handle cannot create directory
    }
    current_id = "0";
  }
}


const std::string BlockStoreFlat::get_last_id() {
  std::string tmp;
  if (!dump_dir.empty()) {
    FileHandle dir = fs::open(dump_dir);
    if (dir.isDirectory()) {
      for (FileIterator it = dir.begin(); it != dir.end(); ++it) {
        if (get_next_id(tmp) != *it) {
          // Inconsistency in block store
          // TODO: handle inconsistency
        }
        tmp = *it;
      }

    } else {
      // Not a directory
      // TODO: handle
    }
  }
  return tmp;
}


const std::vector<uint8_t> &BlockStoreFlat::get(const std::string id) {
  std::vector<uint8_t> result;
  std::string filename = dump_dir + current_id + ".dmp";
  FileHandle fh = fs::open(filename);
  if (fh.exists())
  {
    auto content = fh.readFile();
    std::copy(content.begin(), content.end(), result.begin());
    // TODO: make more clever read
  }
 return result;
}

}
}
