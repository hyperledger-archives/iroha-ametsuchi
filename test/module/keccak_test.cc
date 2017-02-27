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

#include <gtest/gtest.h>
extern "C" {
#include <SimpleFIPS202.h>
}

TEST(HashTest, sha3_256) {
  // yes, I know that his test can not be linked with: 
  //   undefined reference to `SHA3_256(unsigned char*, unsigned char const*, unsigned long)'
  // fix this, the code is correct
  const unsigned char expected[] = {0xa7, 0xff, 0xc6, 0xf8, 0xbf, 0x1e, 0xd7, 0x66, 0x51, 0xc1, 0x47, 0x56, 
                                    0xa0, 0x61, 0xd6, 0x62, 0xf5, 0x80, 0xff, 0x4d, 0xe4, 0x3b, 0x49, 0xfa, 
                                    0x82, 0xd8, 0xa , 0x4b, 0x80, 0xf8, 0x43, 0x4a};

  unsigned char output[32];

  SHA3_256(output, (const unsigned char*) "", 0);

  for(int i=0; i<32; i++){
    ASSERT_EQ(output[i], expected[i]) << "result and expected differ at index " << i;
  }
}
