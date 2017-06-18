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

#ifndef AMETSUCHI_AMETSUCHI_HPP
#define AMETSUCHI_AMETSUCHI_HPP

#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include <experimental/optional>

namespace ametsuchi {
  class Ametsuchi {
   public:
    // factory
    static std::unique_ptr<Ametsuchi> create();

    // block_store
    virtual void insert_block(uint64_t block_id,
                              const std::vector<uint8_t> &blob) = 0;
    virtual void erase_block(uint64_t block_id) = 0;
    virtual std::vector<uint8_t> get_block(uint64_t block_id) = 0;

    // index
    virtual void insert_block_index(uint64_t block_id,
                                    const std::string& hash) = 0;
    virtual void insert_tx_index(int tx_id, const std::string& hash,
                                 uint64_t block_id) = 0;
    virtual std::experimental::optional<uint64_t> get_block_id_by_block_hash(
      const std::string &hash) = 0;
    // TODO merge into one method?
    virtual std::experimental::optional<uint64_t> get_block_id_by_tx_hash(const std::string &hash) = 0;
    virtual std::experimental::optional<uint64_t> get_tx_id(const std::string &hash) = 0;

    // wsv
    virtual void add_account(const std::string &account_id,
                             uint8_t quorum,
                             uint32_t status) = 0;
    virtual void add_signatory(const std::string &account_id,
                                   const std::string &public_key) = 0;
    virtual void remove_signatory() = 0;
    virtual void add_peer(const std::string &account_id,
                              const std::string &address,
                              uint32_t state) = 0;
    virtual std::vector<std::string> get_peers(bool committed) = 0;

    virtual void commit_block() = 0;
    virtual void commit_tx() = 0;
    virtual void rollback_block() = 0;
    virtual void rollback_tx() = 0;
  };
}  // namespace ametsuchi

#endif  // AMETSUCHI_AMETSUCHI_HPP
