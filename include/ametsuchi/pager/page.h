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

#pragma once

#include <ametsuchi/globals.h>
#include <math.h>

namespace ametsuchi {
namespace pager {

// file structures don't need to has padded bytes
// struct {char, int} -> 5 bytes (with pack) or 8 bytes (without pack)
#pragma pack(push, 1)

enum { AMETSUCHI_PAGE_SIZE = 4096, };
struct Page {uint8_t page[AMETSUCHI_PAGE_SIZE];};

enum class PageType : uint8_t { DATA_ITEM, MERKLE_NODE, INDEX_NODE };

enum class SpanningItem : uint8_t {
  NO_SPANNING,
  SINGLE_PREFIX,
  SINGLE_SUFFIX,
  SINGLE_PREFIX_AND_SUFFIX,
  SINGLE_INFIX
};

struct PageDirectory {
  uint16_t offset;
  uint16_t length;
};

struct FileHeader {
  uint8_t fmt_header[7];
  uint8_t fmt_version;
  uint8_t page_size_exponent;
  uint16_t block_merkle_tree_width;
  uint8_t block_merkle_tree_height;
  uint16_t global_merkle_tree_width;
  uint16_t hash_algo;
};

constexpr FileHeader default_file_header = {
    "天地",  //fmt_header[7]
    0,      //fmt_version
    0,      //page_size_exponent
    0,      //block_merkle_tree_width
    0,      //block_merkle_tree_height
    0,      //global_merkle_tree_width
    0,      //hash_algo
};

struct PageHeader {
  PageType page_type;
  SpanningItem spanning_items;
  uint16_t entry_count;

  uint32_t page_directory_count;
  PageDirectory* data_items;
};

// restore previous value
#pragma pack(pop)

inline size_t page_idx(std::size_t offset) {
  return offset / AMETSUCHI_PAGE_SIZE;
}

inline size_t page_ceil(std::size_t size) {
  return ceil(size / AMETSUCHI_PAGE_SIZE);
}

}  // namespace pager
}  // namespace ametsuchi
