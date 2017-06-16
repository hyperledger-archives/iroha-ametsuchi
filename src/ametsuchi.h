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

#ifndef AMETSUCHI_AMETSUCHIIMPL_H
#define AMETSUCHI_AMETSUCHIIMPL_H


#include <memory>
#include <block_store.h>
#include <block_index.h>
#include <tx_index.h>
#include <wsv.h>
#include <ametsuchi/ametsuchi.h>

namespace ametsuchi {

class AmetsuchiImpl : public Ametsuchi {
 public:
  void insert(const std::string &blob) override;
  std::string get_by_height(uint32_t height) override;
  std::string get_by_block_hash(std::string hash) override;
  std::string get_by_tx_hash(std::string hash) override;
  void erase_by_height(uint32_t height) override;
  void add_account() override;
  void add_signatory(std::string account_id, std::string string) override;
  void add_peer() override;
  std::vector<std::string> get_peers(bool committed) override;
 private:
  std::unique_ptr<block_store::BlockStore> block_store_;
  std::unique_ptr<block_index::BlockIndex> block_index_;
  std::unique_ptr<tx_index::TxIndex> tx_index_;
  std::unique_ptr<wsv::WSV> wsv_;
};

}

#endif //AMETSUCHI_AMETSUCHIIMPL_H
