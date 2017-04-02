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

#include <gtest/gtest.h>
#include <lmdb.h>
#include <spdlog/spdlog.h>

static auto console = spdlog::stdout_color_mt("lmdb");

class LMDB_Test : public ::testing::Test {
 protected:
  virtual void TearDown() {
    remove((lmdb + "data.mdb").c_str());
    remove((lmdb + "lock.mdb").c_str());
  }

  virtual void TearUp() {
    remove((lmdb + "data.mdb").c_str());
    remove((lmdb + "lock.mdb").c_str());
  }

  std::string lmdb = "/tmp/tx_index_test";

  LMDB_Test() {
    int res;

    mdb_env_create(&env);
    mdb_env_set_mapsize(env, 1024L * 1024 * 2);  // 2 MB
    mdb_env_set_maxdbs(env, 3);                  // we have only 3 databases

    // create index directory
    res = mkdir(lmdb.c_str(), 0700);
    if (res == -1) {
      if (errno != EEXIST) {
        console->critical("can not create {} folder", lmdb);
        exit(res);
      }
    }

    res = mdb_env_open(env, lmdb.c_str(), MDB_FIXEDMAP, 0600);
    if (res) {
      switch (res) {
        case MDB_VERSION_MISMATCH: {
          console->info("MDB_VERSION_MISMATCH");
          break;
        }
        case MDB_INVALID: {
          console->info("MDB_INVALID");
          break;
        }
        case ENOENT: {
          console->info("ENOENT");
          break;
        }
        case EACCES: {
          console->info("EACCES");
          break;
        }
        case EAGAIN: {
          console->info("EAGAIN");
          break;
        }
      }
      console->critical("can not open index db, errcode: {}", res);
      exit(res);
    }
  }

  void open_tx() {
    int res;
    if ((res = mdb_txn_begin(env, NULL, 0, &append_tx))) {
      console->critical("mdb_tx_begin");
      exit(res);
    }

    if ((res = mdb_dbi_open(append_tx, "TEST1",
                            MDB_DUPSORT | MDB_DUPFIXED | MDB_CREATE,
                            &dbi_index1))) {
      console->critical("mdb_dbi_open add_creator");
      exit(res);
    }

    if ((res = mdb_cursor_open(append_tx, dbi_index1, &cursor_1))) {
      console->critical("mdb_cursor_open 1");
      exit(res);
    }
  }

  void commit_tx() {
    mdb_cursor_close(cursor_1);
    mdb_txn_commit(append_tx);
    mdb_env_stat(env, &mst);
  }

  MDB_env *env;
  MDB_stat mst;
  MDB_txn *append_tx;
  MDB_dbi dbi_index1;
  MDB_cursor *cursor_1;
};


TEST_F(LMDB_Test, AppendSameKeyAndRead) {
  int res;
  open_tx();

  // WRITE
  MDB_val key;
  key.mv_data = (void *)"helloworld";
  key.mv_size = 10;

  for (int i = 0; i < 255; i++) {
    MDB_val val;

    val.mv_data = &i;
    val.mv_size = sizeof(int);

    if ((res = mdb_cursor_put(cursor_1, &key, &val, 0))) {
      console->critical("mdb_cursor_put");
      ASSERT_EQ(res, 0);
    }
  }

  key.mv_data = (void *)"hell0world";
  key.mv_size = 10;

  for (int i = 0; i < 255; i++) {
    MDB_val val;

    val.mv_data = &i;
    val.mv_size = sizeof(int);

    if ((res = mdb_cursor_put(cursor_1, &key, &val, 0))) {
      console->critical("mdb_cursor_put");
      ASSERT_EQ(res, 0);
    }
  }

  commit_tx();

  // READ
  MDB_val val;
  MDB_txn *read_tx;
  MDB_dbi read_dbi;
  MDB_cursor *cursor;

  mdb_txn_begin(env, NULL, MDB_RDONLY, &read_tx);
  mdb_dbi_open(read_tx, "TEST1", MDB_DUPSORT | MDB_DUPFIXED, &read_dbi);
  mdb_cursor_open(read_tx, read_dbi, &cursor);

  if ((res = mdb_cursor_get(cursor, &key, &val, MDB_SET))) {
    mdb_cursor_close(cursor);
    mdb_txn_abort(read_tx);
    ASSERT_EQ(res, 0) << "key not found";
  }

  int result = 0;
  int res1;
  do {
    int index = *reinterpret_cast<int *>(val.mv_data);
    ASSERT_EQ(index, result++);
    if ((res1 = mdb_cursor_get(cursor, &key, &val, MDB_NEXT_DUP))) {
      if (res1 == EINVAL) {
        console->critical("EINVAL");
      }
    }
  } while (res1 == 0);
  mdb_cursor_close(cursor);
  mdb_txn_abort(read_tx);

  ASSERT_EQ(result, 255);
}
