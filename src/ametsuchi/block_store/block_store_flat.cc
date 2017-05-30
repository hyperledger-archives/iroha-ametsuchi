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

#include <iostream>
#include <cppfs/fs.h>
#include <cppfs/FileHandle.h>

namespace fs = cppfs::fs;
using cppfs::FileHandle;

namespace ametsuchi {
namespace block_store {

BlockStoreFlat::BlockStoreFlat() {

  // Restore block, read last block
  current_id = get_last_id();
  // Init flat block store, might throw exception

  auto p = fs::open("./dump");
  if (p.exists()) // does path p actually exist?
  {
    // There exist block store
    if (p.isDirectory())
    {
      // Everything is ok
      std::cout << "Directory exist ";
    }
  }
  else
  {
    if (!p.createDirectory())
    {
      std::cout << "Error creating dir";
      // throw
    }
    else
    {

    }
    // Create new block store
  }
}

std::string BlockStoreFlat::append(const std::string index, const std::vector<uint8_t> &block) {

  std::string file_name;
  file_name.reserve(100);

  file_name += ".dat";
  // Write block to binary file
  auto file_path = fs::open(file_name);
  if (!file_path.exists())
  {
    // New file will be created
    FILE* pfile;

    pfile = fopen(file_name, "wb");
    auto res = fwrite(block.data(), sizeof(uint8_t), block.size(),
                           pfile);
    fflush(pfile);



  }
  else
  {
    // Already exists, something is wrong
    // Answer iroha status
  }
  // Write in one dir dump

  // Other dir may contain links to this dir

  //

}

BlockStoreFlat::~BlockStoreFlat() {

}

const std::vector<uint8_t> &BlockStoreFlat::get(const std::string hash) {
  return <#initializer#>;
}
const std::string BlockStoreFlat::get_last_id() {
  std::string tmp;
  if (!dump_dir.empty())
  {

    auto apk_path = fs::open(dump_dir);

//    for(fs::recursive_directory_iterator i(apk_path); i != end; ++i)
//    {
//      const fs::path cp = (*i);
//      tmp = cp.stem().string();
//    }
    // TODO implement
//    apk_path.traverse([]())
    // Get from tmp the id by splitting
     //tmp = tmp - ''
  }

  return tmp;
}
const std::string BlockStoreFlat::get_next_id() {
  std::string new_id;
  if (!current_id.empty())
  {
    std::string::size_type  sz;
    long li_dec = std::stol(current_id, &sz);
    ++li_dec;
    char buf[100];
    sprintf(buf, "%016li", li_dec);

  }

}

}
}
