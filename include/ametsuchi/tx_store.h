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

#include <string>
#include "status.h"
#include "cache.h"

namespace ametsuchi {

class AppendableFile;
class Serialzier;

template <typename T, Serializer serializer>
class TXStore {
 public:
  explicit TXStore(const std::string &path);

  Status append(const T& tx);


 private:
  std::unique_ptr<AppendableFile> file_;
  std::unique_ptr<Serializer> serializer_;


};

static struct Entry{
  ByteArray key;
  ByteArray value;
};

static class PKIndex{
  Cache<uint64_t, Entry> cache;
};

}

#endif  // AMETSUCHI_TX_STORE_H
