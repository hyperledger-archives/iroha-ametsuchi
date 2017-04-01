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
#include <ametsuchi/tx_store/iterator.h>
#include <spdlog/spdlog.h>

#define PUBKEY_LENGTH 44

namespace ametsuchi {

static auto console = spdlog::stdout_color_mt("ametsuchi");

Ametsuchi::Ametsuchi(const std::string &db_folder) : path_(db_folder) {
  int status = mkdir(db_folder.c_str(), 0700);
  if (-1 == status) {
    if (errno == EEXIST) {
      console->debug("{} exists", db_folder);
      // ok
    } else {
      console->critical("mkdir returned code {} for {}", errno, db_folder);
      throw exception::IOError("mkdir");
    }
  }

  tx_ = std::make_unique<tx_store::Array>(path_ + "/ledger");

  create_tx_index();
}

void Ametsuchi::append(const ByteArray &blob) {
  size_t index = tx_->append(blob);

  append_index(blob);
}

void Ametsuchi::append(const std::vector<ByteArray> &batch) {
  tx_->append_batch(batch);

  // update index
  for (auto &&v : batch) {
    append_index(v);
  }
}


void Ametsuchi::commit() {
  console->debug("commit()");
  tx_->commit();

  // commit old transaction
  mdb_cursor_close(cursor_1);
  mdb_cursor_close(cursor_2);
  mdb_cursor_close(cursor_3);
  mdb_txn_commit(append_tx);
  mdb_env_stat(env, &mst);

  // create new append transaction
  open_append_tx();
}


void Ametsuchi::rollback() {
  tx_->rollback();
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

  c_key.mv_size = PUBKEY_LENGTH;
  c_key.mv_data = reinterpret_cast<void *>(pubKey.c_str());

  mdb_txn_begin(env, NULL, 0, &read_tx);
  mdb_dbi_open(read_tx, "add_creator", MDB_DUPSORT | MDB_DUPFIXED, &read_dbi);
  mdb_cursor_open(read_tx, read_dbi, &cursor);


  if ((res = mdb_cursor_get(cursor, &c_key, &c_val, MDB_SET)) ==
      MDB_NOTFOUND) {                 // no items with given key
    return std::vector<ByteArray>{};  // return empty vector
  }

  do {
    size_t index = *reinterpret_cast<size_t *>(c_val.mv_data);
    ret.push_back(tx_->get(index));

    if ((res = mdb_cursor_get(cursor, &c_key, &c_val, MDB_NEXT_DUP)) ==
        MDB_NOTFOUND) {  // no more items with given key
      break;
    }
  } while (!res);

  return ret;
}

void Ametsuchi::create_tx_index() {
  int res;

  mdb_env_create(&env);
  mdb_env_set_mapsize(env, 1024L * 1024 * 2);  // 2 MB
  mdb_env_set_maxdbs(env, 3);                  // we have only 3 databases

  std::string lmdb = path_ + "/tx_index";

  // create index directory
  res = mkdir(lmdb.c_str(), 0600);
  if (res == -1) {
    if (errno != EEXIST) {
      console->critical("can not create {} folder", lmdb);
      throw exception::Exception("create_tx_index mkdir");
    }
  }

  res = mdb_env_open(env, lmdb.c_str(), MDB_FIXEDMAP, 0700);
  if (res) {
    console->critical("can not open index db");
    throw exception::Exception("create_tx_index mdb_env_open");
  }

  open_append_tx();
}

/**
 * Create separate databases:
 * 1. (tx_type="Add", creator pubkey)
 * 2. (tx_type="Transfer", sender pubkey)
 * 3. (tx_type="Transfer", receiver pubkey)
 *
 * each allows key duplicates
 */
void Ametsuchi::open_append_tx() {
  int res;
  if ((res = mdb_txn_begin(env, NULL, 0, &append_tx))) {
    console->critical("mdb_tx_begin");
    exit(res);
  }

  if ((res = mdb_dbi_open(append_tx, "add_creator",
                          MDB_DUPSORT | MDB_DUPFIXED | MDB_CREATE,
                          &dbi_index1))) {
    console->critical("mdb_dbi_open add_creator");
    exit(res);
  }

  if ((res = mdb_dbi_open(append_tx, "transfer_sender",
                          MDB_DUPSORT | MDB_DUPFIXED | MDB_CREATE,
                          &dbi_index2))) {
    console->critical("mdb_dbi_open transfer_sender");
    exit(res);
  }

  if ((res = mdb_dbi_open(append_tx, "transfer_receiver",
                          MDB_DUPSORT | MDB_DUPFIXED | MDB_CREATE,
                          &dbi_index3))) {
    console->critical("mdb_dbi_open transfer_receiver");
    exit(res);
  }

  if ((res = mdb_cursor_open(append_tx, dbi_index1, &cursor_1))) {
    console->critical("mdb_cursor_open 1");
    exit(res);
  }

  if ((res = mdb_cursor_open(append_tx, dbi_index2, &cursor_2))) {
    console->critical("mdb_cursor_open 2");
    exit(res);
  }

  if ((res = mdb_cursor_open(append_tx, dbi_index3, &cursor_3))) {
    console->critical("mdb_cursor_open 3");
    exit(res);
  }
}


void Ametsuchi::append_index(const ByteArray &blob) {
  auto tx = iroha::GetTransaction(blob.data());

  if (tx->command_type() == iroha::Command::Add) {
    // update index
    MDB_val c_key, c_val;
    int res;

    auto &&creator = tx->creatorPubKey();
    c_key.mv_data = reinterpret_cast<void *>(creator->data()->data());
    c_key.mv_size = creator->data()->size();

    c_val.mv_data = &index;
    c_val.mv_size = sizeof(index);

    if ((res = mdb_cursor_put(cursor_1, &c_key, &c_val, 0))) {
      console->critical("error while appending {}", tx->hash());
      abort_append_tx();
      exit(res);
    }

  } else if (tx->command_type() == iroha::Command::Transfer) {
    // update transfer_sender index
    {
      MDB_val c_key, c_val;
      int res;

      auto &&sender = tx->command_as_Transfer()->sender();
      c_key.mv_data = reinterpret_cast<void *>(sender->data()->data());
      c_key.mv_size = sender->data()->size();

      c_val.mv_data = &index;
      c_val.mv_size = sizeof(index);

      if ((res = mdb_cursor_put(cursor_2, &c_key, &c_val, 0))) {
        console->critical("error while appending {} transfer_sender",
                          tx->hash());
        abort_append_tx();
        exit(res);
      }
    }

    // update transfer_receiver index
    {
      MDB_val c_key, c_val;
      int res;

      auto &&receiver = tx->command_as_Transfer()->receiver();
      c_key.mv_data = reinterpret_cast<void *>(receiver->data()->data());
      c_key.mv_size = receiver->data()->size();

      c_val.mv_data = &index;
      c_val.mv_size = sizeof(index);

      if ((res = mdb_cursor_put(cursor_3, &c_key, &c_val, 0))) {
        console->critical("error while appending {} transfer_sender",
                          tx->hash());
        abort_append_tx();
        exit(res);
      }
    }
  }
}

void Ametsuchi::abort_append_tx() {
  mdb_cursor_close(cursor_1);
  mdb_cursor_close(cursor_2);
  mdb_cursor_close(cursor_3);
  mdb_txn_abort(append_tx);
}

}  // namespace ametsuchi
