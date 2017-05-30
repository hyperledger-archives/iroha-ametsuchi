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

#include <boost/filesystem.hpp>
#include <iostream>
#include <boost/filesystem/fstream.hpp>

namespace fs = boost::filesystem;

namespace ametsuchi {
namespace block_store {

BlockStoreFlat::BlockStoreFlat() {

  // Restore block, read last block
  current_id = get_last_id();
  // Init flat block store, might throw exception

  fs::path p("./dump");
  if (fs::exists(p)) // does path p actually exist?
  {
    // There exist block store
    if (fs::is_directory(p))
    {
      // Everything is ok
      std::cout << "Directory exist ";
    }
  }
  else
  {
    boost::system::error_code returnedError;
    fs::create_directory(p, returnedError);
    if (returnedError)
    {
      std::cout << "Error creating dir " << returnedError.message();
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
  auto file_path = fs::current_path() / file_name;
  if (fs::exists(file_path))
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

    fs::path apk_path(dump_dir);
    fs::recursive_directory_iterator end;

    for(fs::recursive_directory_iterator i(apk_path); i != end; ++i)
    {
      const fs::path cp = (*i);
      tmp = cp.stem().string();
    }
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