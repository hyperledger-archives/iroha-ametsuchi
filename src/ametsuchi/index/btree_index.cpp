//
// Created by hdalpha on 08.03.17.
//

#include "btree_index.h"



btree_index::btree_index(const std::string& fname) {


    //TODO: Handle errors
    mdb_env_create(&env);
    mdb_env_set_maxreaders(env, 1);
    mdb_env_set_mapsize(env, 10485760);
    mdb_env_open(env, fname.c_str(), MDB_FIXEDMAP, 0664);




    // MDB_MAPASYNC - acync write tot disk, can lose last transaction during a system crash
    // MDB_NORDAHEAD - no read ahead, os should care itself
    // MDB_WRITEMAP
    // MDB_FIXEDMAP
  /*  catch()
    {
        mdb_env_close(env);
    }*/


}

template <typename K, typename V>
V btree_index::get(K&& key) {
    MDB_val c_key, c_val;
    MDB_txn *txn;

    c_key.mv_size = sizeof(K);
    c_key.mv_data = reinterpret_cast<void*>(&key);

    mdb_txn_begin(env, NULL, 0, &txn);
    mdb_dbi_open(txn, NULL, 0, &dbi);

    mdb_get(txn, dbi, &c_key, &c_val);

    mdb_txn_abort(txn);
    return reinterpret_cast<V*>(&c_val.mv_data);
}

template <typename K, typename V>
int btree_index::insert(K&& key, V&& val) {
    MDB_val c_key, c_val;
    MDB_txn *txn;


    mdb_txn_begin(env, NULL, 0, &txn);
    mdb_dbi_open(txn, NULL, 0, &dbi);


    c_key.mv_size = sizeof(K);
    c_key.mv_data = reinterpret_cast<void*>(&key);

    c_val.mv_size = sizeof(V);
    c_val.mv_data = reinterpret_cast<void*>(&val);

    mdb_put(txn, dbi, &c_key, &c_val, MDB_NOOVERWRITE);

    mdb_txn_commit(txn);
    // Check is in valid state
    mdb_env_stat(env, &mst);


}

btree_index::~btree_index() {
    mdb_dbi_close(env, dbi);
    mdb_env_close(env);
}
