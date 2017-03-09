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

#ifndef LIBLMDB_BTREE_INDEX_H
#define LIBLMDB_BTREE_INDEX_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string>
#include "lmdb.h"

namespace ametsuchi {
namespace index {

template <typename K, typename V>
class BTreeIndex {

  MDB_env *env;
  MDB_dbi dbi;
  MDB_stat mst;

 public:
  btree_index(const std::string &fname);

  int insert(K &&key, V &&val);

  V get(K &&keyid);

  ~btree_index();
};


}  // namespace index
}  // namespace ametsuchi


#endif  // LIBLMDB_BTREE_INDEX_H
