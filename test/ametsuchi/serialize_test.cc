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

#include <ametsuchi/globals.h>
#include <ametsuchi/serializer.h>
#include <ametsuchi/status.h>
#include <sstream>
#include <gtest/gtest.h>

namespace ametsuchi {
namespace serialize {

TEST(Serialize, FunctionTest){
  ByteArray memory(1024);

  uint64_t a = 0x1111111111111111;
  uint32_t b = 0x22222222;
  uint16_t c = 0x3333;
  uint8_t d = 0x44;
  ByteArray key = {5, 6, 7, 8};
  ByteArray value = {4, 3, 2, 1};

  ametsuchi::byte_t *ptr = memory.data();

  // serialize
  put(ptr, key);
  put(ptr, a);
  put(ptr, b);
  put(ptr, c);
  put(ptr, d);
  put(ptr, value);

  // invalidate values
  a = 0;
  b = 0;
  c = 0;
  d = 0;
  key = {0};
  value = {0};

  // deserialize
  const ametsuchi::byte_t *p = memory.data();  // pointer to the beginning

  get(&key, p);
  get(&a, p);
  get(&b, p);
  get(&c, p);
  get(&d, p);
  get(&value, p);

  ASSERT_EQ(a, 0x1111111111111111);
  ASSERT_EQ(b, 0x22222222);
  ASSERT_EQ(c, 0x3333);
  ASSERT_EQ(d, 0x44);
  ASSERT_EQ(key, ByteArray({5, 6, 7, 8}));
  ASSERT_EQ(value, ByteArray({4, 3, 2, 1}));
}

TEST(Serialize, MacrosTest) {
  ByteArray memory(1024);

  uint64_t a = 0x1111111111111111;
  uint32_t b = 0x22222222;
  uint16_t c = 0x3333;
  uint8_t d = 0x44;
  ByteArray key = {5, 6, 7, 8};
  ByteArray value = {4, 3, 2, 1};

  uint8_t* ptr = memory.data();

  // serialize
  PUT_BYTE_ARRAY(ptr, key);
  PUT_UINT(ptr, a, uint64_t);
  PUT_UINT(ptr, b, uint32_t);
  PUT_UINT(ptr, c, uint16_t);
  PUT_UINT(ptr, d, uint8_t);
  PUT_BYTE_ARRAY(ptr, value);

  // invalidate values
  a = 0;
  b = 0;
  c = 0;
  d = 0;
  key = {0};
  value = {0};

  // deserialize
  const uint8_t* p = memory.data();  // pointer to the beginning

  GET_BYTE_ARRAY(key, p);
  GET_UINT(&a, p, uint64_t);
  GET_UINT(&b, p, uint32_t);
  GET_UINT(&c, p, uint16_t);
  GET_UINT(&d, p, uint8_t);
  GET_BYTE_ARRAY(value, p);

  ASSERT_EQ(a, 0x1111111111111111);
  ASSERT_EQ(b, 0x22222222);
  ASSERT_EQ(c, 0x3333);
  ASSERT_EQ(d, 0x44);
  ASSERT_EQ(key, ByteArray({5, 6, 7, 8}));
  ASSERT_EQ(value, ByteArray({4, 3, 2, 1}));
}

TEST(Serialize, SstreamTest){
  std::stringstream s;

  uint64_t a = 0x1111111111111111;
  uint32_t b = 0x22222222;
  uint16_t c = 0x3333;
  uint8_t d = 0x44;
  ByteArray key = {5, 6, 7, 8};
  ByteArray value = {4, 3, 2, 1};

  s << a << b << c << d << key.size();
//  s.write(reinterpret_cast<unsigned char*>(key.data()), key.size());

}

}  // namespace serialize
}  // namespace ametsuchi
