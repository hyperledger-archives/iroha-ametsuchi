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

#include <ametsuchi/file/rw_file.h>
#include <ametsuchi/globals.h>
#include <ametsuchi/serializer.h>

namespace ametsuchi {
namespace file {


TEST(FileTest, RWFileTest) {
  std::string filename = "/tmp/test1";
  RWFile f(filename);
  f.remove();
  auto opened = f.open();

  ASSERT_TRUE(opened);
  ASSERT_TRUE(f.is_opened());

  ASSERT_EQ(f.size(), 0) << "file is not empty";

  auto s = f.append(ByteArray({1, 2, 3}));
  ASSERT_EQ(f.size() - 3, s) << "incorrect offset";

  s = f.append(ByteArray({3, 4, 3, 4}));
  ASSERT_EQ(f.size() - 4, s) << "incorrect offset";
  ASSERT_EQ(f.size(), 7) << "incorrect size";
  ASSERT_EQ(f.position(), 7) << "incorrect position";

  // cursor now in the end
  s = f.write(ByteArray({0xff, 0xff, 0xff}));
  ASSERT_EQ(s, 3) << "size of written bytes differs";
  ASSERT_EQ(f.position(), 10) << "cursor is not in the end";

  f.seek_to_start();
  ASSERT_EQ(f.position(), 0) << "cursor is not at the start";

  s = f.write(ByteArray({0xfe, 0xfa, 0xfe}));
  ASSERT_EQ(s, 3) << "size of written bytes differs";

  // close and then open
  f.close();
  ASSERT_FALSE(f.is_opened());

  opened = f.open();
  ASSERT_TRUE(opened);
  ASSERT_TRUE(f.is_opened());

  ByteArray res = f.read(f.size());
  ASSERT_EQ(ByteArray({0xfe, 0xfa, 0xfe, 3, 4, 3, 4, 0xff, 0xff, 0xff}), res)
      << "wrong answer";

  // testing seek
  for (uint32_t i = 0; i < f.size(); i++) {
    f.seek(i);
    ASSERT_EQ(i, f.position());
  }

  f.seek(0);
  res = f.read(3);
  ASSERT_EQ(ByteArray({0xfe, 0xfa, 0xfe}), res);

  f.seek_to_end();
  ASSERT_EQ(f.position(), f.size());

  ASSERT_TRUE(f.can_write());
  ASSERT_TRUE(f.can_read());


  f.close();
}

TEST(FileTest, ReadOnlyFileTest) {
  std::string filename = "/tmp/test1";

  RWFile rwf(filename);
  rwf.remove();
  rwf.open();
  rwf.write(ByteArray({0xfe, 0xfa, 0xfe, 3, 4, 3, 4, 0xff, 0xff, 0xff}));
  rwf.close();

  ROFile f(filename);

  ASSERT_TRUE(f.can_read());
  ASSERT_FALSE(f.can_write());
  ASSERT_FALSE(f.is_opened());

  auto opened = f.open();

  ASSERT_TRUE(opened);
  ASSERT_TRUE(f.can_read());

  ByteArray res = f.read(3);
  ASSERT_EQ(ByteArray({0xfe, 0xfa, 0xfe}), res);

  ASSERT_EQ(f.position(), 3);

  res = f.read(4);
  ASSERT_EQ(ByteArray({3, 4, 3, 4}), res);
  ASSERT_EQ(f.position(), 7);

  f.seek(1);
  res = f.read(2);
  ASSERT_EQ(ByteArray({0xfa, 0xfe}), res);

  f.close();
}

TEST(FileTest, HugeFileWriteRead) {
  size_t size = 100000;

  std::string filename = "/tmp/test1";
  RWFile writeFile(filename);
  writeFile.remove();

  if (writeFile.open()) {
    for (uint32_t i = 0; i < size; i++) {
      ByteArray memory(4);
      uint8_t *ptr = memory.data();
      PUT_UINT(ptr, i, uint32_t);
      uint32_t a = 0;
      GET_UINT(&a, ptr, uint32_t);
      writeFile.append(memory);
    }
  }
  writeFile.close();

  ROFile readFile(filename);
  if (readFile.open()) {
    for (uint32_t i = 0; i < size; i++) {
      ByteArray memory = readFile.read(4);
      uint32_t res;
      uint8_t *ptr = memory.data();
      //      std::cout << readFile.size() << std::endl; // if this is
      //      uncommented no failure occurs:)
      GET_UINT(&res, ptr, uint32_t);
      ASSERT_EQ(i, res);
    }
  }
  readFile.close();
}

}
}
