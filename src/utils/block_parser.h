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


#ifndef AMETSUCHI_BLOCK_PARSER_H
#define AMETSUCHI_BLOCK_PARSER_H

#include <string>
#include <vector>

namespace utils {

class BlockParser {
 public:
  BlockParser(std::string raw);
  std::string get_hash();
  std::vector<std::string> get_transactions();

 private:
  std::string hash;
  std::vector<std::string> transactions;
  void split(std::string raw);
};
}


#endif  // AMETSUCHI_BLOCK_PARSER_H
