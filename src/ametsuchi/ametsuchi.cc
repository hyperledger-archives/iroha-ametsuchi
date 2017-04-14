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

// static auto console = spdlog::stdout_color_mt("ametsuchi");

// TODO: remove this
#define AMETSUCHI_TREES_TOTAL 8


namespace ametsuchi {


Ametsuchi::Ametsuchi(const std::string &db_folder)
    : path_(db_folder), tree(AMETSUCHI_BLOCK_SIZE), tx_store(), wsv() {
  // initialize database:
  // create folder, create all handles and btrees
  // in case of any errors print error to stdout and exit
  init();
}

merkle::hash_t Ametsuchi::append(const flatbuffers::Vector<uint8_t> *blob) {
  auto tx = flatbuffers::GetRoot<iroha::Transaction>(blob->data());

  MDB_val c_key, c_val;
  int res;

  // 1. Append to TX_store
  tx_store.append(blob);
  // 2. Update WSV
  wsv.update(blob);

  merkle::hash_t h;
  assert(tx->hash()->size() == merkle::HASH_LEN);
  std::copy(tx->hash()->begin(), tx->hash()->end(), &h[0]);
  tree.push(h);
  return tree.root();
}

merkle::hash_t Ametsuchi::append(
    const std::vector<flatbuffers::Vector<uint8_t> *> &batch) {
  for (auto t : batch) {
    append(t);

    auto tx = flatbuffers::GetRoot<iroha::Transaction>(t);

    merkle::hash_t h;
    assert(tx->hash()->size() == merkle::HASH_LEN);
    std::copy(tx->hash()->begin(), tx->hash()->end(), &h[0]);

    tree.push(h);
  }

  return tree.root();
}


void Ametsuchi::commit() {
  // commit old transaction
  tx_store.close_cursors();
  wsv.close_cursors();
  mdb_txn_commit(append_tx_);
  mdb_env_stat(env, &mst);

  // create new append transaction
  init_append_tx();
}


void Ametsuchi::rollback() {
  abort_append_tx();
  init_append_tx();
}

// std::vector<const MDB_val> Ametsuchi::getAddTxByCreator(const std::string
// &pubKey) {
// TODO
/*
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
 */
//}


void Ametsuchi::abort_append_tx() {
  tx_store.close_cursors();
  wsv.close_cursors();
  if (append_tx_) mdb_txn_abort(append_tx_);
}


void Ametsuchi::init() {
  int res;

  // create database directory
  if ((res = mkdir(path_.c_str(), 0700))) {
    if (res == EEXIST) {
      console->debug("folder with database exists");
    } else {
      AMETSUCHI_CRITICAL(res, EACCES);
      AMETSUCHI_CRITICAL(res, ELOOP);
      AMETSUCHI_CRITICAL(res, EMLINK);
      AMETSUCHI_CRITICAL(res, ENAMETOOLONG);
      AMETSUCHI_CRITICAL(res, ENOENT);
      AMETSUCHI_CRITICAL(res, ENOSPC);
      AMETSUCHI_CRITICAL(res, ENOTDIR);
      AMETSUCHI_CRITICAL(res, EROFS);
    }
  }

  // create environment
  if ((res = mdb_env_create(&env))) {
    AMETSUCHI_CRITICAL(res, MDB_VERSION_MISMATCH);
    AMETSUCHI_CRITICAL(res, MDB_INVALID);
    AMETSUCHI_CRITICAL(res, ENOENT);
    AMETSUCHI_CRITICAL(res, EACCES);
    AMETSUCHI_CRITICAL(res, EAGAIN);
  }

  // set maximum mmap size. Must be multiple of OS page size (4 KB).
  // max size of the database (!!!)
  if ((res = mdb_env_set_mapsize(env, AMETSUCHI_MAX_DB_SIZE))) {
    AMETSUCHI_CRITICAL(res, EINVAL);
  }

  // set number of databases in single file
  // TODO: change maxdbs
  if ((res = mdb_env_set_maxdbs(env, AMETSUCHI_TREES_TOTAL))) {
    AMETSUCHI_CRITICAL(res, EINVAL);
  }

  // create database environment
  if ((res = mdb_env_open(env, path_.c_str(), MDB_FIXEDMAP, 0700))) {
    AMETSUCHI_CRITICAL(res, MDB_VERSION_MISMATCH);
    AMETSUCHI_CRITICAL(res, MDB_INVALID);
    AMETSUCHI_CRITICAL(res, ENOENT);
    AMETSUCHI_CRITICAL(res, EACCES);
    AMETSUCHI_CRITICAL(res, EAGAIN);
  }

  // stats about db
  mdb_env_stat(env, &mst);

  // initialize
  init_append_tx();
}


void Ametsuchi::init_append_tx() {
  int res;

  // begin "append" transaction
  if ((res = mdb_txn_begin(env, NULL, 0, &append_tx_))) {
    AMETSUCHI_CRITICAL(res, MDB_PANIC);
    AMETSUCHI_CRITICAL(res, MDB_MAP_RESIZED);
    AMETSUCHI_CRITICAL(res, MDB_READERS_FULL);
    AMETSUCHI_CRITICAL(res, ENOMEM);
  }
  tx_store.init(append_tx_);
  // create database instances for each tree, open cursors for each tree, save
  // them in map this.trees_: [name] => std::pair<dbi, cursor>
  wsv.init(append_tx_);

  // TODO: remove it
  // assert(AMETSUCHI_TREES_TOTAL == trees_.size());

  // stats about db
  mdb_env_stat(env, &mst);
}


std::vector<AM_val> Ametsuchi::accountGetAllAssets(
    const flatbuffers::String *pubKey, bool uncommitted) {
  return wsv.accountGetAllAssets(pubKey, uncommitted, env);
}


AM_val Ametsuchi::accountGetAsset(const flatbuffers::String *pubKey,
                                  const flatbuffers::String *ln,
                                  const flatbuffers::String *dn,
                                  const flatbuffers::String *cn,
                                  bool uncommitted) {
  return wsv.accountGetAsset(pubKey, ln, dn, cn, uncommitted, env);
}


Ametsuchi::~Ametsuchi() {
  abort_append_tx();

  tx_store.close_dbi(env);
  wsv.close_dbi(env);
  mdb_env_close(env);
}


merkle::hash_t Ametsuchi::merkle_root() { return tree.root(); }


}  // namespace ametsuchi
