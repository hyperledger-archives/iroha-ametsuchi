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
#ifndef AMETSUCHI_BTREE_INDEX_H
#define AMETSUCHI_BTREE_INDEX_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string>

#include <lmdb.h>

#include <ametsuchi/file/file.h>

namespace ametsuchi {
namespace index {

template <typename K, typename V>
class BTreeIndex {
 public:
  BTreeIndex(const std::string &fname);

  void insert(K key, V val);

  const V get(K keyid);

  ~BTreeIndex();

  void insertBatch(std::vector<K> keys, std::vector<V> vals);

  std::vector<V> getBatch(K startKey, uint64_t num);

 private:
  MDB_env *env;
  MDB_dbi dbi;
  MDB_stat mst;
  static const u_int64_t map_size = 1024L * 1024 * 1; // 1 MB
};



template <typename K, typename V>
BTreeIndex<K, V>::BTreeIndex(const std::string &fname) {
  mdb_env_create(&env);
  //mdb_env_set_maxreaders(env, 1);
  mdb_env_set_mapsize(env, map_size);
  int res = mdb_env_open(env, fname.c_str(), MDB_FIXEDMAP, 0664);
  if (res) {
    // TODO: handle exception
    printf("res is not NULL when opening ");
    exit(res);
  }

  // Open database in environment
  MDB_txn *txn;
  mdb_txn_begin(env, NULL, 0, &txn);
  mdb_dbi_open(txn, NULL, 0, &dbi);
  mdb_txn_commit(txn);
}


template <typename K, typename V>
const V BTreeIndex<K, V>::get(K key) {
  MDB_val c_key, c_val;
  MDB_txn *txn;


  c_key.mv_size = sizeof(K);
  c_key.mv_data = (&key);


  mdb_txn_begin(env, NULL, 0, &txn);
  mdb_dbi_open(txn, NULL, 0, &dbi);

  mdb_get(txn, dbi, &c_key, &c_val);

  mdb_txn_abort(txn);
  return *reinterpret_cast<V*>(c_val.mv_data);
}

template <>
const ByteArray BTreeIndex<ametsuchi::file::offset_t, ByteArray >::get(ametsuchi::file::offset_t key){
  MDB_val c_key, c_val;
  MDB_txn *txn;

  c_key.mv_size = sizeof(ametsuchi::file::offset_t);
  c_key.mv_data = (&key);

  mdb_txn_begin(env, NULL, 0, &txn);
  mdb_dbi_open(txn, NULL, 0, &dbi);

  mdb_get(txn, dbi, &c_key, &c_val);

  ByteArray tmp;
  auto tmp_ptr = reinterpret_cast<byte_t*>(c_val.mv_data);
  tmp.assign(tmp_ptr, tmp_ptr+c_val.mv_size);
  mdb_txn_abort(txn);

  return tmp;
};

template <typename K, typename V>
std::vector<V> BTreeIndex<K, V>::getBatch(K startKey, uint64_t num){
  MDB_val c_key, c_val;
  MDB_txn *txn;
  MDB_cursor *cursor;

  c_key.mv_data = (&startKey);
  c_key.mv_size = sizeof(K);

  mdb_txn_begin(env, NULL, MDB_RDONLY, &txn);
  mdb_cursor_open(txn, dbi, &cursor);

  std::vector<V> result;
  int rc = mdb_cursor_get(cursor, &c_key, &c_val, MDB_SET_KEY);
  // TODO: Handle exceptions
  if (rc == MDB_NOTFOUND){
    printf("\n KEY NOT FOUND \n");
    mdb_cursor_close(cursor);
    mdb_txn_abort(txn);
    return result;
  }
  result.push_back(*reinterpret_cast<V*>(c_val.mv_data));

  for (int i = 1; i < num; ++i) {
    rc = mdb_cursor_get(cursor, &c_key, &c_val, MDB_NEXT);
    if (rc) {
      // TODO: Handle exceptions
      printf("\n MDB NEXT doesn't work \n");
      mdb_cursor_close(cursor);
      mdb_txn_abort(txn);
      return result;
    }
    printf("\n DATA KEY %d %d \n", *reinterpret_cast<V*>(c_val.mv_data), *reinterpret_cast<K*>(c_key.mv_data));
    result.push_back(*reinterpret_cast<V*>(c_val.mv_data));
  }
  mdb_cursor_close(cursor);
  mdb_txn_abort(txn);
  return result;
}


template <typename K, typename V>
void BTreeIndex<K, V>::insert(K key, V val) {
  MDB_val c_key, c_val;
  MDB_txn *txn;
  int res = 0;

  // TODO: Handle exceptions
  res = mdb_txn_begin(env, NULL, 0, &txn);
  res = mdb_dbi_open(txn, NULL, 0, &dbi);

  c_key.mv_size = sizeof(K);
  c_key.mv_data = (&key);

  c_val.mv_size = sizeof(V);
  c_val.mv_data = (&val);

  mdb_put(txn, dbi, &c_key, &c_val, 0);

  mdb_txn_commit(txn);
  // Update database stats
  mdb_env_stat(env, &mst);
}

template<>
void BTreeIndex<ametsuchi::file::offset_t, ByteArray>::insert(ametsuchi::file::offset_t key, ByteArray val) {

  MDB_val c_key, c_val;
  MDB_txn *txn;
  int res = 0;


  res = mdb_txn_begin(env, NULL, 0, &txn);

  res = mdb_dbi_open(txn, NULL, 0, &dbi);


  c_key.mv_size = sizeof(ametsuchi::file::offset_t);
  c_key.mv_data = (&key);

  c_val.mv_size = val.size()* sizeof(byte_t);
  c_val.mv_data = (val.data());

  mdb_put(txn, dbi, &c_key, &c_val, 0);

  mdb_txn_commit(txn);

  // Check if it is in valid state
  mdb_env_stat(env, &mst);
}

template <typename K, typename V>
void BTreeIndex<K, V>::insertBatch(std::vector<K> keys, std::vector<V> vals)
{
  MDB_val c_key, c_val;
  MDB_txn *txn;
  MDB_cursor *cursor;

  mdb_txn_begin(env, NULL, 0, &txn);
  mdb_cursor_open(txn, dbi, &cursor);

  for (int i=0;i<vals.size();i++) {
    c_key.mv_data = (&keys[i]);
    c_key.mv_size = sizeof(K);

    c_val.mv_size = sizeof(V);
    c_val.mv_data = (&vals[i]);

    int res = mdb_cursor_put(cursor, &c_key, &c_val, 0);
    if (res) {
      mdb_cursor_close(cursor);
      mdb_txn_abort(txn);
      // TODO:Handle exception
      return;
    }
  }
  mdb_cursor_close(cursor);
  mdb_txn_commit(txn);

};


template <typename K, typename V>
BTreeIndex<K, V>::~BTreeIndex() {
  mdb_dbi_close(env, dbi);
  mdb_env_close(env);
}

}  // namespace index
}  // namespace ametsuchi


#endif  // AMETSUCHI_BTREE_INDEX_H
