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
#include <dirent.h>

namespace fs = cppfs::fs;
using cppfs::FileHandle;
using cppfs::FileIterator;

namespace ametsuchi {
namespace block_store {


std::string BlockStoreFlat::append(const std::vector<uint8_t> &block) {
  std::string next_id = get_next_id(current_id);
  std::string file_name = dump_dir + "/" + next_id;
  // Write block to binary file
  auto fh = fs::open(file_name);
  if (!fh.exists()) {
    // New file will be created
    FILE *pfile;
    pfile = fopen(file_name.c_str(), "wb");
    auto res = fwrite(block.data(), sizeof(uint8_t), block.size(), pfile);
    fflush(pfile);
    fclose(pfile);

    // Update internals, release lock
    current_id = next_id;
    return current_id;
  } else {
    // Already exists, something is wrong
    // Answer iroha status
    // TODO: handle this case
    std::cout << "File name already exist " << std::endl;
    return std::string();
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
      current_id = check_consitency();
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
      std::cout << "Error creating directory " << std::endl;
    }
    current_id = "0";
    dump_dir = path;
  }
}


const std::string BlockStoreFlat::check_consitency() {
  std::string tmp = "0";
  if (!dump_dir.empty()) {
    FileHandle dir = fs::open(dump_dir);
    if (dir.isDirectory()) {

      // Directory iterator:
      struct dirent **namelist;
      auto n = scandir(dump_dir.c_str(), &namelist, NULL, alphasort);
      if (n<0) {
        // TODO: handle error
      }
      else{
        uint i = 1;
        while(++i < n){
          if (get_next_id(tmp) != namelist[i]->d_name) {
           // TODO: handle Inconsistent state
          }
          tmp = namelist[i]->d_name;
          free(namelist[i]);
        }
        free(namelist);
      }


    } else {
      // Not a directory
      // TODO: handle
      std::cout << "Not a directory "  <<std::endl;
    }
  }
  return tmp;
}


const std::vector<uint8_t> BlockStoreFlat::get(const std::string id) {
  std::string filename = dump_dir + "/" + id;
  FileHandle fh = fs::open(filename);
  if (fh.exists()) {
    auto f_size = fh.size();
    std::vector<uint8_t > buf(f_size);
    FILE* pfile = fopen(filename.c_str(), "rb");
    fread(&buf[0], sizeof(uint8_t), f_size, pfile);
    fclose(pfile);
    return buf;

  } else {
    std::cout << "No file with this id " << std::endl;
    // TODO: handle not found
    return std::vector<uint8_t>();
  }

}

const std::string BlockStoreFlat::get_next_id(std::string old_id) {
  std::string new_id(16, '\0');
  if (!old_id.empty()) {
    std::string::size_type sz;
    auto li_dec = std::stoll(old_id, &sz);
    ++li_dec;
    sprintf(&new_id[0], "%016lli", li_dec);
  }
  return new_id;
}

}
}
