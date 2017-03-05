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

#ifndef AMETSUCHI_FIXED_TABLE_H
#define AMETSUCHI_FIXED_TABLE_H

#include "table.h>

namespace ametsuchi{
namespace table{

class FixedTable: public Table{
 public:
  FixedTable(/*TODO add parameters*/);
 private:
  size_t key_size;
  size_t value_size;

  size_t entry_size;
};

}
}

#endif  // AMETSUCHI_FIXED_TABLE_H
