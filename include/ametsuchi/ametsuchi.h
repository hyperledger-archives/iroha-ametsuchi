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

#ifndef AMETSUCHI_AMETSUCHI_H
#define AMETSUCHI_AMETSUCHI_H

#include <string>
#include <vector>

namespace ametsuchi {

class Ametsuchi {
 public:
  std::unique_ptr<Ametsuchi> create();

  // Block store
  virtual void insert(const std::string &blob) = 0;
  virtual std::string get_by_height(uint32_t height) = 0;
  virtual std::string get_by_block_hash(std::string hash) = 0;
  virtual std::string get_by_tx_hash(std::string hash) = 0;
  virtual void erase_by_height(uint32_t height) = 0;

  // WSV
  virtual void add_account() = 0;
  virtual void add_signatory(std::string account_id, std::string) = 0;
  virtual void add_peer() = 0;

  virtual std::vector<std::string> get_peers(bool committed) = 0;
};

}

#endif //AMETSUCHI_AMETSUCHI_H
