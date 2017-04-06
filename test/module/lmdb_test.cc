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
#include <ametsuchi/generated/commands_generated.h>
#include <ametsuchi/generated/primitives_generated.h>
#include <ametsuchi/generated/transaction_generated.h>
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

class Ametsuchi_Test : public ::testing::Test {
 protected:
  virtual void TearDown() { system(("rm -rf " + folder).c_str()); }

  std::string folder = "/tmp/ametsuchi/";
  ametsuchi::Ametsuchi ametsuchi_;

  Ametsuchi_Test() : ametsuchi_(ametsuchi::Ametsuchi(folder)){}
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

TEST_F(Ametsuchi_Test, AssetTest) {
  // create command
  flatbuffers::FlatBufferBuilder builder;

  auto asset_name = builder.CreateString("Dollar");
  auto domain_name = builder.CreateString("USA");
  auto ledger_name = builder.CreateString("ledger1");
  auto creator_pub_key = builder.CreateString("1");
  auto asset_create = iroha::CreateAssetCreate(builder, asset_name, domain_name,
                                               ledger_name, creator_pub_key);
  // create signature
  uint8_t sign_data[] = {0x1, 0x2, 0x3};
  auto signature_field = builder.CreateVector(sign_data, 3);
  uint64_t timestamp = 123823;
  auto signature = iroha::CreateSignature(builder, creator_pub_key, signature_field, timestamp);

  // create transaction
  std::vector<flatbuffers::Offset<iroha::Signature>> signatures_vector;
  signatures_vector.push_back(signature);
  auto signatures = builder.CreateVector(signatures_vector.data(), 1);
  auto transaction = iroha::CreateTransaction(builder, creator_pub_key, iroha::Command::AssetCreate, asset_create.Union(), signatures);

  builder.Finish(transaction);

  uint8_t *buf = builder.GetBufferPointer();
  int size = builder.GetSize();

//  auto transaction_object = flatbuffers::GetRoot<iroha::Transaction>(buf);
  std::vector<uint8_t > transaction_vector{buf, buf+size};
  ametsuchi_.append(transaction_vector);
}