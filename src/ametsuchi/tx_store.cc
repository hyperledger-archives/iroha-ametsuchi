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

#include <ametsuchi/exception.h>
#include <ametsuchi/generated/asset_generated.h>
#include <ametsuchi/generated/transaction_generated.h>
#include <ametsuchi/tx_store.h>

namespace ametsuchi {


void TxStore::append(const flatbuffers::Vector<uint8_t> *blob) {
  auto tx = flatbuffers::GetRoot<iroha::Transaction>(blob->data());

  MDB_val c_key, c_val;
  int res;
  // 1. append TX in the end of TX STORE
  {
    c_key.mv_data = &(++tx_store_total);
    c_key.mv_size = sizeof(tx_store_total);
    c_val.mv_data = (void *)blob->data();
    c_val.mv_size = blob->size();

    if ((res = mdb_cursor_put(trees_.at("tx_store").second, &c_key, &c_val,
                              MDB_NOOVERWRITE | MDB_APPEND))) {
      AMETSUCHI_CRITICAL(res, MDB_KEYEXIST);
      AMETSUCHI_CRITICAL(res, MDB_MAP_FULL);
      AMETSUCHI_CRITICAL(res, MDB_TXN_FULL);
      AMETSUCHI_CRITICAL(res, EACCES);
      AMETSUCHI_CRITICAL(res, EINVAL);
    }
  }

  // 2. insert record into index depending on the command
  {
    auto creator = tx->creatorPubKey();
    switch (tx->command_type()) {
      case iroha::Command::AssetCreate:
      case iroha::Command::AssetAdd:
      case iroha::Command::AssetRemove:
      case iroha::Command::AssetTransfer:
        put_creator_into_tree(trees_.at("index_asset_creator").second, creator,
                              tx_store_total);
        break;
      case iroha::Command::AccountAdd:
      case iroha::Command::AccountAddSignatory:
      case iroha::Command::AccountRemove:
      case iroha::Command::AccountRemoveSignatory:
      case iroha::Command::AccountSetUseKeys:
        put_creator_into_tree(trees_.at("index_account_creator").second,
                              creator, tx_store_total);
        break;
      case iroha::Command::PeerAdd:
      case iroha::Command::PeerChangeTrust:
      case iroha::Command::PeerRemove:
      case iroha::Command::PeerSetActive:
      case iroha::Command::PeerSetTrust:
        put_creator_into_tree(trees_.at("index_peer_creator").second, creator,
                              tx_store_total);
        break;
      default:
        throw exception::InvalidTransaction::WRONG_COMMAND;
    }
  }

  // 3. insert record into index_transfer_sender and index_transfer_receiver
  if (tx->command_type() == iroha::Command::AssetTransfer) {
    // update index_transfer_sender
    auto cmd = tx->command_as_AssetTransfer();
    c_key.mv_data = (void *)(cmd->sender()->data());
    c_key.mv_size = cmd->sender()->size();

    if ((res = mdb_cursor_put(trees_.at("index_transfer_sender").second, &c_key,
                              &c_val, 0))) {
      AMETSUCHI_CRITICAL(res, MDB_MAP_FULL);
      AMETSUCHI_CRITICAL(res, MDB_TXN_FULL);
      AMETSUCHI_CRITICAL(res, EACCES);
      AMETSUCHI_CRITICAL(res, EINVAL);
    }

    // update index_transfer_receiver
    c_key.mv_data = (void *)(cmd->receiver()->data());
    c_key.mv_size = cmd->receiver()->size();

    if ((res = mdb_cursor_put(trees_.at("index_transfer_receiver").second,
                              &c_key, &c_val, 0))) {
      AMETSUCHI_CRITICAL(res, MDB_MAP_FULL);
      AMETSUCHI_CRITICAL(res, MDB_TXN_FULL);
      AMETSUCHI_CRITICAL(res, EACCES);
      AMETSUCHI_CRITICAL(res, EINVAL);
    }
  }
}

void TxStore::init(MDB_txn *append_tx) {
  append_tx_ = append_tx;

  // autoincrement_key => tx (NODUP)
  trees_["tx_store"] =
      init_btree(append_tx_, "tx_store", MDB_CREATE | MDB_INTEGERKEY);

  // [pubkey] => [autoincrement_key] (DUP)
  trees_["index_asset_creator"] =
      init_btree(append_tx_, "index_asset_creator",
                 MDB_DUPSORT | MDB_DUPFIXED | MDB_CREATE);

  // [pubkey] => [autoincrement_key] (DUP)
  trees_["index_account_creator"] =
      init_btree(append_tx_, "index_account_creator",
                 MDB_DUPSORT | MDB_DUPFIXED | MDB_CREATE);

  // [pubkey] => [autoincrement_key] (DUP)
  trees_["index_peer_creator"] =
      init_btree(append_tx_, "index_peer_creator",
                 MDB_DUPSORT | MDB_DUPFIXED | MDB_CREATE);

  // [pubkey] => autoincrement_key (DUP)
  trees_["index_transfer_sender"] =
      init_btree(append_tx_, "index_transfer_sender",
                 MDB_DUPSORT | MDB_DUPFIXED | MDB_CREATE);

  // [pubkey] => autoincrement_key (DUP)
  trees_["index_transfer_receiver"] =
      init_btree(append_tx_, "index_transfer_receiver",
                 MDB_DUPSORT | MDB_DUPFIXED | MDB_CREATE);

  set_tx_total();
  assert(get_trees_total() == trees_.size());
}

void TxStore::close_cursors() {
  for (auto &&e : trees_) {
    MDB_cursor *cursor = e.second.second;
    if (cursor) mdb_cursor_close(cursor);
  }
}

TxStore::TxStore() {}

TxStore::~TxStore() {}

void TxStore::set_tx_total() {
  MDB_val c_key, c_val;
  int res;

  MDB_cursor *cursor = trees_.at("tx_store").second;

  if ((res = mdb_cursor_get(cursor, &c_key, &c_val, MDB_LAST))) {
    if (res == MDB_NOTFOUND) {
      tx_store_total = 0u;
    }
    AMETSUCHI_CRITICAL(res, EINVAL);
  } else {
    tx_store_total = *reinterpret_cast<size_t *>(c_key.mv_data);
  }
}
void TxStore::close_dbi(MDB_env *env) {
  for (auto &&it : trees_) {
    auto dbi = it.second.first;
    mdb_dbi_close(env, dbi);
  }
}
uint32_t TxStore::get_trees_total() {
  TX_STORE_TREES_TOTAL = 6;
  return TX_STORE_TREES_TOTAL;
}


void TxStore::put_creator_into_tree(MDB_cursor *cursor,
                                    const flatbuffers::String *acc_pub_key,
                                    size_t &tx_store_total) {
  MDB_val c_key, c_val;
  int res;

  c_key.mv_data = (void *)(acc_pub_key->data());
  c_key.mv_size = acc_pub_key->size();
  c_val.mv_data = &tx_store_total;
  c_val.mv_size = sizeof(tx_store_total);

  if ((res = mdb_cursor_put(cursor, &c_key, &c_val, 0))) {
    AMETSUCHI_CRITICAL(res, MDB_MAP_FULL);
    AMETSUCHI_CRITICAL(res, MDB_TXN_FULL);
    AMETSUCHI_CRITICAL(res, EACCES);
    AMETSUCHI_CRITICAL(res, EINVAL);
  }
}


std::vector<AM_val> TxStore::getTxByCreator(const std::string &tree_name,
                                            const flatbuffers::String *pubKey,
                                            bool uncommitted, MDB_env *env) {
  MDB_val c_key, c_val;
  MDB_cursor *cursor;
  MDB_txn *tx;
  int res;

  // query asset by public key
  c_key.mv_data = (void *)pubKey->data();
  c_key.mv_size = pubKey->size();

  if (uncommitted) {
    cursor = trees_.at(tree_name).second;
    tx = append_tx_;
  } else {
    // create read-only transaction, create new RO cursor
    if ((res = mdb_txn_begin(env, NULL, MDB_RDONLY, &tx))) {
      AMETSUCHI_CRITICAL(res, MDB_PANIC);
      AMETSUCHI_CRITICAL(res, MDB_MAP_RESIZED);
      AMETSUCHI_CRITICAL(res, MDB_READERS_FULL);
      AMETSUCHI_CRITICAL(res, ENOMEM);
    }

    if ((res = mdb_cursor_open(tx, trees_.at(tree_name).first, &cursor))) {
      AMETSUCHI_CRITICAL(res, EINVAL);
    }
  }

  // if sender has no such tx, then it is pub_key
  if ((res = mdb_cursor_get(cursor, &c_key, &c_val, MDB_SET))) {
    if (res == MDB_NOTFOUND) return std::vector<AM_val>{};
    AMETSUCHI_CRITICAL(res, EINVAL);
  }

  std::vector<AM_val> ret;
  // index tree has transactions. try to find asset with the same `pk`
  // iterate over creator's transactions, O(N), where N is number of different
  // transactions,
  MDB_val tx_key, tx_val;
  MDB_cursor *tx_cursor;

  if (uncommitted) {
    tx_cursor = trees_.at("tx_store").second;
  } else {
    if ((res = mdb_cursor_open(tx, trees_.at("tx_store").first, &tx_cursor))) {
      AMETSUCHI_CRITICAL(res, EINVAL);
    }
  }
  do {
    tx_key = c_val;
    if ((res = mdb_cursor_get(tx_cursor, &tx_key, &tx_val, MDB_FIRST))) {
      AMETSUCHI_CRITICAL(res, MDB_NOTFOUND);
      AMETSUCHI_CRITICAL(res, EINVAL);
    }
    ret.push_back(AM_val(tx_val));
    if ((res = mdb_cursor_get(cursor, &c_key, &c_val, MDB_NEXT_DUP))) {
      if (res == MDB_NOTFOUND) break;
      AMETSUCHI_CRITICAL(res, EINVAL);
    }
  } while (res == 0);
  if (!uncommitted) {
    mdb_cursor_close(cursor);
    mdb_cursor_close(tx_cursor);
    mdb_txn_abort(tx);
  }

  return ret;
}
std::vector<AM_val> TxStore::getAssetTxByCreator(
    const flatbuffers::String *pubKey, bool uncommitted, MDB_env *env) {
  return getTxByCreator("index_asset_creator", pubKey, uncommitted, env);
}
std::vector<AM_val> TxStore::getAccountTxByCreator(
    const flatbuffers::String *pubKey, bool uncommitted, MDB_env *env) {
  return getTxByCreator("index_account_creator", pubKey, uncommitted, env);
}
std::vector<AM_val> TxStore::getPeerTxByCreator(
    const flatbuffers::String *pubKey, bool uncommitted, MDB_env *env) {
  return getTxByCreator("index_peer_creator", pubKey, uncommitted, env);
}
std::vector<AM_val> TxStore::getAssetTxBySender(
    const flatbuffers::String *senderKey, bool uncommitted, MDB_env *env) {
  return getTxByCreator("index_transfer_sender", senderKey, uncommitted, env);
}
std::vector<AM_val> TxStore::getAssetTxByReceiver(
    const flatbuffers::String *receiverKey, bool uncommitted, MDB_env *env) {
  return getTxByCreator("index_transfer_receiver", receiverKey, uncommitted,
                        env);
}
}
