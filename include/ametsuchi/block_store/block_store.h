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

#pragma once

#include <string>
#include <vector>

namespace ametsuchi {
namespace block_store{

class BlockStore {
 public:
  virtual std::string append(const std::vector<uint8_t> &block) = 0;
  virtual const std::vector<uint8_t> & get(const std::string id) = 0;
  // Iterators
  //virtual const std::string get_last_id() = 0;
 protected:

  static const std::string get_next_id(std::string old_id) {
    std::string new_id(16, (char)'/0');
    if (!old_id.empty()) {
      std::string::size_type sz;
      auto li_dec = std::stoll(old_id, &sz);
      ++li_dec;
      sprintf(new_id, "%016lli", li_dec);
    }
    return new_id;
  }

};

}
}
