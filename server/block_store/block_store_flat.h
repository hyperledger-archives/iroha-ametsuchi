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

#include "block_store.h"


namespace block_store {
class BlockStoreFlat : public BlockStore {
 public:

  BlockStoreFlat(const std::string &path);
  ~BlockStoreFlat();

  const uint64_t append(const std::vector<uint8_t> &block) override;
  const std::vector<uint8_t> get(uint64_t id) override;
  const uint64_t last_id() override;

  class Iterator;

  Iterator begin();
  Iterator end();

 private:
  uint64_t current_id;
  std::string dump_dir;
  // Get next auto increment
  // Get last consistent id, check iternal consistency of block store
  const uint64_t check_consitency();
  const std::string get_next_id(std::string old_id);
  const std::string id_to_name(uint64_t id);
  const uint64_t name_to_id(std::string name);
};
}


