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

#include <ametsuchi/ametsuchi.h>
#include <ametsuchi/generated/transaction_generated.h>
#include <flatbuffers/flatbuffers.h>
#include <spdlog/spdlog.h>

static auto console = spdlog::stdout_color_mt("ametsuchi");

#define AMETSUCHI_HANDLE(res, err)                                        \
  if (res == err) {                                                       \
    console->critical("{}", mdb_strerror(res));                           \
    console->critical("err in {} at #{} in file {}", __PRETTY_FUNCTION__, \
                      __LINE__, __FILE__);                                \
  }

#define AMETSUCHI_TREES_TOTAL 8
#define AMETSUCHI_DB_SIZE (8L * 1024 * 1024 * 1024)

namespace ametsuchi {


Ametsuchi::Ametsuchi(const std::string &db_folder)
    : path_(db_folder), tx_store_total(0) {
  // initialize database:
  // create folder, create all handles and btrees
  // in case of any errors prints error to stdout and exits
  init();
}

void Ametsuchi::append(const ByteArray &blob) {
  auto tx = iroha::GetTransaction(blob.data());

  MDB_val c_key, c_val;
  int res;

  // 1. append TX in the end of TX STORE
  {
    c_key.mv_data = &(++tx_store_total);
    c_key.mv_size = sizeof(tx_store_total);
    c_val.mv_data = (void *)blob.data();
    c_val.mv_size = blob.size();

    if ((res = mdb_cursor_put(trees_["tx_store"].second, &c_key, &c_val,
                              MDB_NOOVERWRITE | MDB_APPEND))) {
      if (res == MDB_KEYEXIST) {
        console->critical("KEY {} EXISTS IN TX STORE\nentries={}\n",
                          tx_store_total, mst.ms_entries);
      }
      AMETSUCHI_HANDLE(res, MDB_MAP_FULL);
      AMETSUCHI_HANDLE(res, MDB_TXN_FULL);
      AMETSUCHI_HANDLE(res, EACCES);
      AMETSUCHI_HANDLE(res, EINVAL);
      exit(res);
    }
  }

  // 2. insert record into index_add_creator
  {
    auto &&creator = tx->creatorPubKey();
    c_key.mv_data = (void *)(creator->data()->data());
    c_key.mv_size = creator->data()->size();
    c_val.mv_data = &tx_store_total;
    c_val.mv_size = sizeof(tx_store_total);

    if ((res = mdb_cursor_put(trees_["index_add_creator"].second, &c_key,
                              &c_val, 0))) {
      AMETSUCHI_HANDLE(res, MDB_MAP_FULL);
      AMETSUCHI_HANDLE(res, MDB_TXN_FULL);
      AMETSUCHI_HANDLE(res, EACCES);
      AMETSUCHI_HANDLE(res, EINVAL);
      exit(res);
    }
  }

  // 3. insert record into index_transfer_sender and index_transfer_receiver
  if (tx->command_type() == iroha::Command::CmdTransferAsset) {
    // update index_transfer_sender
    {
      auto &&sender = tx->command_as_CmdTransferAsset()->sender();
      c_key.mv_data = (void *)(sender->data()->data());
      c_key.mv_size = sender->data()->size();

      if ((res = mdb_cursor_put(trees_["index_transfer_sender"].second, &c_key,
                                &c_val, 0))) {
        AMETSUCHI_HANDLE(res, MDB_MAP_FULL);
        AMETSUCHI_HANDLE(res, MDB_TXN_FULL);
        AMETSUCHI_HANDLE(res, EACCES);
        AMETSUCHI_HANDLE(res, EINVAL);
        exit(res);
      }
    }

    // update index_transfer_receiver
    {
      auto &&receiver = tx->command_as_CmdTransferAsset()->receiver();
      c_key.mv_data = (void *)(receiver->data()->data());
      c_key.mv_size = receiver->data()->size();

      if ((res = mdb_cursor_put(trees_["index_transfer_receiver"].second,
                                &c_key, &c_val, 0))) {
        AMETSUCHI_HANDLE(res, MDB_MAP_FULL);
        AMETSUCHI_HANDLE(res, MDB_TXN_FULL);
        AMETSUCHI_HANDLE(res, EACCES);
        AMETSUCHI_HANDLE(res, EINVAL);
        exit(res);
      }
    }
  }

  // 4. update WSV
  {
    // if Command == Add
    if (tx->command_type() == iroha::Command::CmdAdd) {
      cmd_add(blob);
    }  // Create<Asset>
    else if (tx->command_type() == iroha::Command::CmdCreateAsset) {
      cmd_create_asset(blob);
    }
  }
}

void Ametsuchi::append(const std::vector<ByteArray> &batch) { return; }


void Ametsuchi::commit() {
  // commit old transaction
  for (auto &&e : trees_) {
    const std::string &name = e.first;
    MDB_dbi &dbi = e.second.first;
    MDB_cursor *cursor = e.second.second;

    mdb_cursor_close(cursor);
  }
  mdb_txn_commit(append_tx_);
  mdb_env_stat(env, &mst);

  // create new append transaction
  open_append_tx();
}


void Ametsuchi::rollback() {
  abort_append_tx();
  open_append_tx();
}

std::vector<ByteArray> Ametsuchi::getAddTxByCreator(const std::string &pubKey) {
  int res;

  std::vector<ByteArray> ret;

  MDB_val c_key, c_val;
  MDB_txn *read_tx;
  MDB_dbi read_dbi;
  MDB_cursor *cursor;

  c_key.mv_size = pubKey.size();
  c_key.mv_data = (void *)pubKey.c_str();

  if ((res = mdb_txn_begin(env, NULL, MDB_RDONLY, &read_tx))) {
    if (res == MDB_PANIC) console->critical("getAddTxByCreator: MDB_PANIC");
    if (res == MDB_MAP_RESIZED)
      console->critical("getAddTxByCreator: MDB_MAP_RESIZED ");
    if (res == MDB_READERS_FULL)
      console->critical("getAddTxByCreator: MDB_READERS_FULL");
    if (res == ENOMEM) console->critical("getAddTxByCreator: ENOMEM");
    exit(res);
  }
  mdb_dbi_open(read_tx, "add_creator", MDB_DUPSORT | MDB_DUPFIXED, &read_dbi);
  mdb_cursor_open(read_tx, read_dbi, &cursor);


  if ((res = mdb_cursor_get(cursor, &c_key, &c_val, MDB_SET)) ==
      MDB_NOTFOUND) {                 // no items with given key
    return std::vector<ByteArray>{};  // return empty vector
  }

  do {
    size_t index = *static_cast<size_t *>(c_val.mv_data);
    ret.push_back(tx_->get(index));

    if ((res = mdb_cursor_get(cursor, &c_key, &c_val, MDB_NEXT_DUP)) ==
        MDB_NOTFOUND) {  // no more items with given key
      break;
    }
  } while (!res);

  return ret;
}


void Ametsuchi::abort_append_tx() {
  for (auto &&e : trees_) {
    MDB_cursor *cursor = e.second.second;
    mdb_cursor_close(cursor);
  }
  mdb_txn_abort(append_tx_);
}


void Ametsuchi::init() {
  int res;

  // create database directory
  if ((res = mkdir(path_.c_str(), 0700))) {
    if (res == EEXIST) {
      console->debug("folder with database exists");
    } else {
      AMETSUCHI_HANDLE(res, EACCES);
      AMETSUCHI_HANDLE(res, ELOOP);
      AMETSUCHI_HANDLE(res, EMLINK);
      AMETSUCHI_HANDLE(res, ENAMETOOLONG);
      AMETSUCHI_HANDLE(res, ENOENT);
      AMETSUCHI_HANDLE(res, ENOSPC);
      AMETSUCHI_HANDLE(res, ENOTDIR);
      AMETSUCHI_HANDLE(res, EROFS);
      exit(res);
    }
  }

  // create environment
  if ((res = mdb_env_create(&env))) {
    AMETSUCHI_HANDLE(res, MDB_VERSION_MISMATCH);
    AMETSUCHI_HANDLE(res, MDB_INVALID);
    AMETSUCHI_HANDLE(res, ENOENT);
    AMETSUCHI_HANDLE(res, EACCES);
    AMETSUCHI_HANDLE(res, EAGAIN);
    exit(res);
  }

  // set maximum mmap size. Must be multiple of OS page size (4 KB).
  // max size of the database (!!!)
  if ((res = mdb_env_set_mapsize(env, AMETSUCHI_DB_SIZE))) {
    AMETSUCHI_HANDLE(res, EINVAL);
    exit(res);
  }

  // set number of databases in single file
  if ((res = mdb_env_set_maxdbs(env, AMETSUCHI_TREES_TOTAL))) {
    AMETSUCHI_HANDLE(res, EINVAL);
    exit(res);
  }

  // create database environment
  if ((res = mdb_env_open(env, path_.c_str(), MDB_FIXEDMAP, 0700))) {
    AMETSUCHI_HANDLE(res, MDB_VERSION_MISMATCH);
    AMETSUCHI_HANDLE(res, MDB_INVALID);
    AMETSUCHI_HANDLE(res, ENOENT);
    AMETSUCHI_HANDLE(res, EACCES);
    AMETSUCHI_HANDLE(res, EAGAIN);
    exit(res);
  }

  // stats about db
  mdb_env_stat(env, &mst);

  // initialize
  open_append_tx();
}


void Ametsuchi::init_btree(const std::string &name, uint32_t flags) {
  int res;
  MDB_dbi dbi;
  MDB_cursor *cursor;
  if ((res = mdb_dbi_open(append_tx_, name.c_str(), flags, &dbi))) {
    AMETSUCHI_HANDLE(res, MDB_NOTFOUND);
    AMETSUCHI_HANDLE(res, MDB_DBS_FULL);
    exit(res);
  }

  if ((res = mdb_cursor_open(append_tx_, dbi, &cursor))) {
    AMETSUCHI_HANDLE(res, EINVAL);
    exit(res);
  }

  // close previous cursor if it is not closed
  auto e = trees_[name];
  if (e.second) {
    mdb_cursor_close(e.second);
  }

  auto tree = std::make_pair(dbi, cursor);
  auto item = std::make_pair(name, tree);
  trees_.insert(item);
}

size_t Ametsuchi::tx_store_size() {
  MDB_val c_key, c_val;
  int res;
  if ((res = mdb_cursor_get(trees_["tx_store"].second, &c_key, &c_val,
                            MDB_LAST)) == MDB_NOTFOUND) {
    return 0u;
  }

  size_t size = *reinterpret_cast<size_t *>(c_key.mv_data);
  return size;
}

void Ametsuchi::open_append_tx() {
  int res;

  // begin "append" transaction
  if ((res = mdb_txn_begin(env, NULL, 0, &append_tx_))) {
    AMETSUCHI_HANDLE(res, MDB_PANIC);
    AMETSUCHI_HANDLE(res, MDB_MAP_RESIZED);
    AMETSUCHI_HANDLE(res, MDB_READERS_FULL);
    AMETSUCHI_HANDLE(res, ENOMEM);
    exit(res);
  }

  // create database instances for each tree, open cursors for each tree, save
  // them in map this.trees_: [name] => std::pair<dbi, cursor>

  // [autoincrement_key] => tx (NODUP)
  init_btree("tx_store", MDB_CREATE | MDB_INTEGERKEY);
  tx_store_total = tx_store_size();  // last index in db as "total entries"

  // [pubkey] => autoincrement_key (DUP)
  init_btree("index_add_creator", MDB_DUPSORT | MDB_DUPFIXED | MDB_CREATE);

  // [pubkey] => autoincrement_key (DUP)
  init_btree("index_transfer_sender", MDB_DUPSORT | MDB_DUPFIXED | MDB_CREATE);

  // [pubkey] => autoincrement_key (DUP)
  init_btree("index_transfer_receiver",
             MDB_DUPSORT | MDB_DUPFIXED | MDB_CREATE);

  // [pubkey] => assets (DUP)
  init_btree("wsv_pubkey_assets", MDB_DUPSORT | MDB_DUPFIXED | MDB_CREATE);

  // [pubkey] => account (NODUP)
  init_btree("wsv_pubkey_account", MDB_CREATE);

  // [ledger_name+domain_name+asset_name] => asset (NODUP)
  init_btree("wsv_assetid_asset", MDB_CREATE);

  // [ip] => peer (NODUP)
  init_btree("wsv_pubkey_peer", MDB_CREATE);

  assert(AMETSUCHI_TREES_TOTAL == trees_.size());
}


inline void Ametsuchi::cmd_add(const ByteArray &blob) {
  MDB_val c_key, c_val;
  int res;

  auto &&tx = iroha::GetTransaction(blob.data());
  auto &&cmd = tx->command_as_CmdAdd();

  // if Add<Account>
  if (cmd->object_type() == iroha::Object::Account) {
    auto account = cmd->object_as_Account();
    auto acc_pubk = account->pubKey();
    c_key.mv_data = (void *)(acc_pubk->data()->data());
    c_key.mv_size = acc_pubk->data()->size();
    c_val.mv_data = &tx_store_total;
    c_val.mv_size = sizeof(tx_store_total);

    if ((res = mdb_cursor_put(trees_["wsv_pubkey_account"].second, &c_key,
                              &c_val, 0))) {
      AMETSUCHI_HANDLE(res, MDB_MAP_FULL);
      AMETSUCHI_HANDLE(res, MDB_TXN_FULL);
      AMETSUCHI_HANDLE(res, EACCES);
      AMETSUCHI_HANDLE(res, EINVAL);
      exit(res);
    }

  }  // if Add<Peer>
  else if (cmd->object_type() == iroha::Object::Peer) {
    auto &&peer = cmd->object_as_Peer();
    auto &&ip = peer->ip();

    // flatbuffers::FlatBufferBuilder fbb;
    // fbb.PushElement(peer);
    // TODO: how to get bytes of peer?


    c_key.mv_data = (void *)(ip->data());
    c_key.mv_size = ip->size();
    c_val.mv_data = 0 /*bytes of peer*/;
    c_val.mv_size = 0 /*size of peer*/;

    if ((res = mdb_cursor_put(trees_["wsv_pubkey_peer"].second, &c_key, &c_val,
                              0))) {
      AMETSUCHI_HANDLE(res, MDB_MAP_FULL);
      AMETSUCHI_HANDLE(res, MDB_TXN_FULL);
      AMETSUCHI_HANDLE(res, EACCES);
      AMETSUCHI_HANDLE(res, EINVAL);
      exit(res);
    }
  } else {
    console->debug(
        "Ametsuchi supports only Add<Peer> and Add<Account> transactions. "
        "WSV is not updated.");
  }
}


inline void Ametsuchi::cmd_create_asset(const ByteArray &blob) {
  MDB_val c_key, c_val;
  int res;

  auto tx = iroha::GetTransaction(blob.data());
  auto cmd = tx->command_as_CmdAdd();

  auto asset = tx->command_as_CmdCreateAsset();

  std::string pk{asset->ledger_name()->begin(), asset->ledger_name()->end()};
  pk += asset->domain_name()->data();
  pk += asset->asset_name()->data();

  c_key.mv_data = (void *)(pk.c_str());
  c_key.mv_size = pk.size();
  c_val.mv_data = (void *)asset->description()->data();
  c_val.mv_size = asset->description()->size();

  if ((res = mdb_cursor_put(trees_["wsv_assetid_asset"].second, &c_key, &c_val,
                            0))) {
    AMETSUCHI_HANDLE(res, MDB_MAP_FULL);
    AMETSUCHI_HANDLE(res, MDB_TXN_FULL);
    AMETSUCHI_HANDLE(res, EACCES);
    AMETSUCHI_HANDLE(res, EINVAL);
    exit(res);
  }
}

}  // namespace ametsuchi
