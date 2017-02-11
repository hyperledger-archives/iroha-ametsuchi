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

#ifndef AMETSUCHI_PAGER_MANAGER_H_
#define AMETSUCHI_PAGER_MANAGER_H_

#include <ametsuchi/globals.h>
#include <stdint.h>

namespace ametsuchi {
namespace pager {

class Manager {
 private:
  extern const int PAGE_SIZE;  // defined in globals.h

  void read(uint64_t pageNumber);

  void allocateNewPage();

};

}  // namespace pager
}  // namespace ametsuchi
#endif  // AMETSUCHI_PAGER_MANAGER_H_
