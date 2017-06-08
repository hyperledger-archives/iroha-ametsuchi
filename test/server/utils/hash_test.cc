/**
 * Copyright Soramitsu Co., Ltd. 2017 All Rights Reserved.
 * http://soramitsu.co.jp
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0/**
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <gtest/gtest.h>
#include <hash.h>

TEST(hash_test, sha3_256_hash_test) {
  std::string my_hash(32, '\0');
  utils::sha3_256((unsigned char *) &my_hash.at(0), nullptr, 0);
  std::string res_hash(64, '\0');
  for (int i = 0; i < my_hash.size(); ++i){
    sprintf(&res_hash[2 * i], "%02x", my_hash[i] & 0xFF);
  }
  ASSERT_STREQ(res_hash.c_str(), "a7ffc6f8bf1ed76651c14756a061d662f580ff4de43b49fa82d80a4b80f8434a");

  std::vector<uint8_t > blob({0x1, 0x2});
  std::string hash(32, '\0');

  utils::sha3_256((unsigned char *) &hash.at(0), blob.data(), 32);
  for (int i = 0; i < hash.size(); ++i){
    printf("%02x", hash[i] & 0xFF);
  }
}
