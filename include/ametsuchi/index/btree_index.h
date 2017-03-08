//
// Created by hdalpha on 08.03.17.
//

#ifndef LIBLMDB_BTREE_INDEX_H
#define LIBLMDB_BTREE_INDEX_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "lmdb.h"
#include <string>


template <typename K, typename V>
class btree_index {

    MDB_env *env;
    MDB_dbi dbi;
    MDB_stat mst;

public:
    btree_index(const std::string& fname);

    int insert(K key, V val);

    V get(K keyid);


};


#endif //LIBLMDB_BTREE_INDEX_H
