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

  void insert(K key, V &&val);

  const V get(K keyid);

  ~BTreeIndex();

 private:
  MDB_env *env;
  MDB_dbi dbi;
  MDB_stat mst;
};



template <typename K, typename V>
BTreeIndex<K, V>::BTreeIndex(const std::string &fname) {
  mdb_env_create(&env);
  mdb_env_set_maxreaders(env, 1);
  mdb_env_set_mapsize(env, 10485760);
  int res = mdb_env_open(env, fname.c_str(), MDB_FIXEDMAP, 0664);
  if (res) {
    printf("res is not NULL when opening ");
    exit(-1);
  }
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
  printf("\n Return specified \n");
  return tmp;
};

template <typename K, typename V>
void BTreeIndex<K, V>::insert(K key, V &&val) {
  MDB_val c_key, c_val;
  MDB_txn *txn;
  int res = 0;

  printf("\n Writing as non specified \n ");
  res = mdb_txn_begin(env, NULL, 0, &txn);

  res = mdb_dbi_open(txn, NULL, 0, &dbi);

  c_key.mv_size = sizeof(K);

  c_key.mv_data = (&key);


  c_val.mv_size = sizeof(V);
  c_val.mv_data = (&val);

  mdb_put(txn, dbi, &c_key, &c_val, 0);


  mdb_txn_commit(txn);

  // Check if it is in valid state
  mdb_env_stat(env, &mst);
}


template<>
void BTreeIndex<ametsuchi::file::offset_t, ByteArray>::insert(ametsuchi::file::offset_t key, ByteArray &&val) {

  printf("Writing as specialized \n ");

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
BTreeIndex<K, V>::~BTreeIndex() {
  mdb_dbi_close(env, dbi);
  mdb_env_close(env);
}

}  // namespace index
}  // namespace ametsuchi


#endif  // AMETSUCHI_BTREE_INDEX_H
