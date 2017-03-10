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

#include <ametsuchi/file/file.h>
#include <ametsuchi/globals.h>
#include <ametsuchi/serializer.h>

namespace ametsuchi {
namespace file {


TEST(FileTest, ReadWriteFileTest) {
  std::string filename = "/tmp/test1";
  ReadWriteFile f(filename);
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

  ReadWriteFile rwf(filename);
  rwf.remove();
  rwf.open();
  rwf.write(ByteArray({0xfe, 0xfa, 0xfe, 3, 4, 3, 4, 0xff, 0xff, 0xff}));
  rwf.close();

  ReadOnlyFile f(filename);

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
  size_t size = 10000000;return;

  std::string filename = "/tmp/test1";
  ReadWriteFile writeFile(filename);
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

  ReadOnlyFile readFile(filename);
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

TEST(FileTest, FileSizeTest) {
  std::string filename = "/tmp/test1";

  ReadWriteFile f(filename);
  f.remove();
  auto opened = f.open();

  ASSERT_TRUE(opened);

  constexpr size_t size = 1024;
  ByteArray data(size);

  f.seek(0);
  f.write(data);

  ASSERT_EQ(f.size(), size);
}

/*
TEST_F(FileTest, PositionTest) {
  Re
  {
    AppendableFile af(filename);
    af.open();

    ASSERT_EQ(af.position(), 0);

    ByteArray wdata = {1, 2, 3};
    af.append(wdata);

    ASSERT_EQ(af.position(), 3);

    wdata = {3, 2, 1};
    af.append(wdata);

    ASSERT_EQ(af.position(), 6);
  }
  {
    SequentialFile sf(filename);
    sf.open();

    ByteArray rdata = sf.read(2, 2);

    ASSERT_EQ(sf.position(), 4);
  }
}

TEST_F(FileTest, SimultaneousReadWrite) {
  AppendableFile af(filename);
  SequentialFile sf(filename);

  af.open();
  sf.open();

  af.append(ByteArray{1, 2, 3});
  ASSERT_EQ(sf.read(2, 0), ByteArray({1, 2}));

  af.append(ByteArray{4, 5, 6});
  ASSERT_EQ(sf.read(2, 2), ByteArray({5, 6}));
}

TEST_F(FileTest, NonexistantFile) {
  SequentialFile sf(filename);
  ASSERT_EQ(sf.open(), false);
}
 */
}
}
