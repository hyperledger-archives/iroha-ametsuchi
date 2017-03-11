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

#ifndef AMETSUCHI_DB_H
#define AMETSUCHI_DB_H

#include <ametsuchi/env.h>
#include <ametsuchi/globals.h>
#include <ametsuchi/table/fixed_table.h>
#include <string>

namespace ametsuchi {

/**
 * Interface to tx_store and world_state
 * @tparam T - flatbuffers, protobuf, any serializable structure
 */
template <typename T>
class Ametsuchi {
 public:
  void open(std::shared_ptr<Env> env) {
    // ENV is declared in "globals.h"
    ENV = env;
  }
  void close();
};
}

#endif  // AMETSUCHI_DB_H
