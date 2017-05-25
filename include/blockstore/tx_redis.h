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

#ifndef AMETSUCHI_TX_REDIS_H
#define AMETSUCHI_TX_REDIS_H

#include <cpp_redis/redis_client.hpp>

namespace blockstore{

class TXRedis{
 public:
  TXRedis(std::string host, size_t port) : host_(host), port_(port){
    client_.connect(host_, port_);
  }

  ~TXRedis();

  bool add_id(std::string hash, size_t id);
  bool add_timestamp(size_t id, size_t timestamp);

  size_t get_id_by_hash(std::string hash);
  std::vector<size_t > get_ids_by_timestamp(size_t timestamp);

 private:
  cpp_redis::redis_client client_;
  std::string host_;
  size_t port_;
};

}

#endif //AMETSUCHI_TX_REDIS_H
