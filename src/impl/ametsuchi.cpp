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

#include <impl/ametsuchi.hpp>
// TODO replace with managers/factories
#include <block_store/backend/flat_file.hpp>
#include <index/backend/redis.hpp>
#include <wsv/backend/postgresql.hpp>

namespace ametsuchi {

  std::unique_ptr<Ametsuchi> Ametsuchi::create() {
    return std::make_unique<impl::Ametsuchi>();
  }

  namespace impl {

    Ametsuchi::Ametsuchi() {
      block_store_ = std::make_unique<block_store::FlatFile>("/tmp/block_store");
      index_ = std::make_unique<index::Redis>("localhost", 6379);
      wsv_ = std::make_unique<wsv::PostgreSQL>("localhost", 5432, "postgres", "");
    }

    Ametsuchi::~Ametsuchi() {

    }

    void Ametsuchi::insert_block(uint64_t block_id,
                                 const std::vector<uint8_t> &blob) {
      block_store_->add(block_id, blob);
    }

    void Ametsuchi::erase_block(uint64_t block_id) {
      block_store_->remove(block_id);
    }

    std::vector<uint8_t> Ametsuchi::get_block(uint64_t block_id) {
      return block_store_->get(block_id);
    }

    void Ametsuchi::insert_block_index(uint64_t block_id,
                                       const std::string &hash) {
      index_->add_blockhash_blockid(hash, block_id);
    }

    void Ametsuchi::insert_tx_index(int tx_id, const std::string &hash,
                                    uint64_t block_id) {
      index_->add_txhash_blockid_txid(hash, block_id, tx_id);
    }

    std::experimental::optional<uint64_t> Ametsuchi::get_block_id_by_block_hash(
      const std::string &hash) {
      return index_->get_blockid_by_blockhash(hash);
    }

    std::experimental::optional<uint64_t> Ametsuchi::get_block_id_by_tx_hash(
      const std::string &hash) {
      return index_->get_blockid_by_txhash(hash);
    }

    std::experimental::optional<uint64_t> Ametsuchi::get_tx_id(const std::string &hash) {
      return index_->get_txid_by_txhash(hash);
    }

    void Ametsuchi::add_account(const std::string &account_id,
                                    uint8_t quorum,
                                    uint32_t status) {
      wsv_->add_account(account_id, quorum, status);
    }

    void Ametsuchi::add_signatory(const std::string &account_id,
                                      const std::string &public_key) {
      wsv_->add_signatory(account_id, public_key);
    }

    void Ametsuchi::remove_signatory() {
      // TODO implement
    }

    void Ametsuchi::add_peer(const std::string &account_id,
                                 const std::string &address,
                                 uint32_t state) {
      wsv_->add_peer(account_id, address, state);
    }

    std::vector<std::string> Ametsuchi::get_peers(bool committed) {
      return wsv_->get_peers(committed);
    }

    void Ametsuchi::commit_block() {
      wsv_->commit_block();
    }

    void Ametsuchi::commit_tx() {
      wsv_->commit_transaction();
    }

    void Ametsuchi::rollback_block() {
      wsv_->rollback_block();
    }

    void Ametsuchi::rollback_tx() {
      wsv_->rollback_transaction();
    }
  }  // namespace impl

}  // namespace ametsuchi
