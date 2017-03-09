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

  f.close();
}
}
}
