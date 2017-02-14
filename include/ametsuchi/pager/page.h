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

#ifndef AMETSUCHI_PAGER_PAGE_H_
#define AMETSUCHI_PAGER_PAGE_H_

#include <stdint.h>  // for uintX_t

namespace ametsuchi {
namespace pager {


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


struct Page {
  PageType pageType;
  SpanningItem spanningItems;
  uint16_t entryCount;

  uint32_t pageDorectoryCount;
  PageDirectory* dataItems;
};

}  // namespace pager
}  // namespace ametsuchi

#endif  // AMETSUCHI_PAGER_PAGE_H_
