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


#include <ametsuchi/file/rw_file_safe.h>
#include <gtest/gtest.h>

namespace ametsuchi {
namespace file {

std::string path = "/tmp/test_rw1";

TEST(RW, Write) {
  RWFileSafe file(path);
  file.clear();

  ASSERT_TRUE(file.open());

  size_t size = 1000;
  ByteArray data(size);
  uint32_t seed = 0;
  for (size_t i = 0; i < size; i++) {
    data[i] = (byte_t)(rand_r(&seed) & 0xFF);
  }

  auto s = file.write(data);
  ASSERT_EQ(s, size);
  ASSERT_EQ(file.size(), s);

  ROFile r(path);
  r.open();
  ASSERT_EQ(data, r.read(size));
}


TEST(RW, Append) {
  RWFileSafe file(path);
  file.clear();

  ASSERT_TRUE(file.open());

  size_t size = 1000;
  ByteArray data(size);
  uint32_t seed = 0;
  for (size_t i = 0; i < size; i++) {
    data[i] = (byte_t)(rand_r(&seed) & 0xFF);
  }

  auto s = file.append(data);
  ASSERT_EQ(s, 0);
  ASSERT_EQ(file.size(), size);

  ROFile r(path);
  r.open();
  ASSERT_EQ(data, r.read(size));
}

}
}
