//
// Created by hdalpha on 08.03.17.
//

#include <ametsuchi/index/btree_index.h>
namespace ametsuchi {
namespace index {


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
V BTreeIndex<K, V>::get(K &&key) {
  MDB_val c_key, c_val;
  MDB_txn *txn;


  c_key.mv_size = sizeof(K);
  c_key.mv_data = (&key);


  mdb_txn_begin(env, NULL, 0, &txn);
  mdb_dbi_open(txn, NULL, 0, &dbi);

  mdb_get(txn, dbi, &c_key, &c_val);

  mdb_txn_abort(txn);
  return *(V *)c_val.mv_data;
  // return 1;
}

template <typename K, typename V>
void BTreeIndex<K, V>::insert(K &&key, V &&val) {
  MDB_val c_key, c_val;
  MDB_txn *txn;
  int res = 0;


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

template <typename K, typename V>
BTreeIndex<K, V>::~BTreeIndex() {
  mdb_dbi_close(env, dbi);
  mdb_env_close(env);
}

}//namespace index
}//namespace ametsuchi
