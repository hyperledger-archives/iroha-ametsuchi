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

class RWS : public ::testing::Test {
 protected:
  virtual void TearDown() {
    remove(path.c_str());
    remove(wal_path.c_str());
  }
  std::string path = "/tmp/test_rw1";
  std::string wal_path = "/tmp/test_rw1.log";
};


TEST_F(RWS, Write) {
  remove(path.c_str());
  remove(wal_path.c_str());

  RWFileSafe file(path, wal_path);

  ASSERT_TRUE(file.open());

  size_t size = 10000;
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

  file.close();
  r.close();
}


TEST_F(RWS, Append) {
  remove(path.c_str());
  remove(wal_path.c_str());

  RWFileSafe file(path, wal_path);

  ASSERT_TRUE(file.open());

  for (uint32_t j = 0; j < 255; j++) {
    size_t size = 100000;
    ByteArray data(size);
    uint32_t seed = 0;
    for (size_t i = 0; i < size; i++) {
      data[i] = (byte_t)(rand_r(&seed) & 0xFF);
    }

    auto s = file.append(data);
    ASSERT_EQ(file.size() - size, s);

    ROFile r(path);
    r.open();
    r.seek(s);
    ASSERT_EQ(data, r.read(size));
  }
}

TEST_F(RWS, RecoveryRandomWrite) {
  remove(path.c_str());
  remove(wal_path.c_str());

  RWFile file(path);
  ASSERT_TRUE(file.open());

  ByteArray data{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  ByteArray transaction(5, 0xff);

  file.write(data);
  ASSERT_EQ(file.size(), data.size());
  file.close();

  RWFileSafe f(path, wal_path);
  ASSERT_TRUE(f.open());

  // TODO(@warchant): how to test recovery engine using just its interface?
  f.seek(5);
  auto s = f.write(transaction);
  ASSERT_EQ(s, transaction.size());

  ROFile r(path);
  ASSERT_TRUE(r.open());

  ByteArray expected{0, 1, 2, 3, 4, 0xff, 0xff, 0xff, 0xff, 0xff,
                     0, 1, 2, 3, 4, 5,    6,    7,    8,    9};

  ASSERT_EQ(r.read(data.size()), expected);
}
}
}
