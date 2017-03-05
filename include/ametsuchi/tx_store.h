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

#ifndef AMETSUCHI_TX_STORE_H
#define AMETSUCHI_TX_STORE_H

#include <generated/tx_store_generated.h>
#include <string>
#include "cache.h"
#include "file/file.h"
#include "status.h"

namespace ametsuchi {

class AppendableFile;
class Serialzier;

template <typename T, Serializer<T> serializer>
class TXStore {
 public:
  explicit TXStore(const std::string& path);

  Status append(const T& tx);

 private:
  AppendableFile file_;

  uint64_t total_tags;
};
}

#endif  // AMETSUCHI_TX_STORE_H
