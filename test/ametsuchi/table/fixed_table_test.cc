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

#include <ametsuchi/globals.h>
#include <ametsuchi/table/fixed_table.h>
#include <ametsuchi/table/table.h>

namespace ametsuchi {
namespace table {


class FileTest : public ::testing::Test {
 protected:
  virtual void TearDown() { remove(filename.c_str()); }

  const std::string filename = "/tmp/test_db";
};

#pragma pack(push)
#pragma pack(1)
struct ExampleFB {
  char str[16];
  uint64_t i;
  double d;
};
#pragma pack(pop)

// 15 char s + ending \0
#define STR_16(c) #c #c #c #c #c #c #c #c #c #c #c #c #c #c #c
#define FB(c, i, d) \
  ExampleFB { STR_16(c), i, d }

TEST_F(FileTest, AppendGetTest) {
  FixedTable<ExampleFB> table(filename);
  uint64_t i = 0x12345678;
  double d = 1234567e+89;
  table.append(FB(A, i, d));
  auto fb = table.get(0);

  ASSERT_STREQ(fb.str, STR_16(A));
  ASSERT_EQ(fb.i, i);
  ASSERT_EQ(fb.d, d);
}

TEST_F(FileTest, AppendGetBatchTest) {
  FixedTable<ExampleFB> table(filename);
  uint64_t i[] = {0x12345678, 0x87654321, 0xfedcba9876543210,
                  0x123456789abcdef};
  double d[] = {1234567e+89, 9876543e+21, 0xfedcba9876543, 0x123456789abcde};
  std::vector<ExampleFB> base_fbs = {FB(A, i[0], d[0]), FB(B, i[1], d[1]),
                                     FB(C, i[2], d[2]), FB(D, i[3], d[3])};
  table.appendBatch(base_fbs);
  auto received_fbs = table.getBatch(base_fbs.size(), 0);

  ASSERT_EQ(received_fbs.size(), base_fbs.size());

  auto it1 = base_fbs.begin();
  auto it2 = received_fbs.begin();
  for (; it1 != base_fbs.end(); ++it1, ++it2) {
    ASSERT_STREQ(it2->str, it1->str);
    ASSERT_EQ(it2->i, it1->i);
    ASSERT_EQ(it2->d, it1->d);
  }
}

TEST_F(FileTest, AppendReplaceGetTest) {
  // Remove when required method will be implemented
  return;
  FixedTable<ExampleFB> table(filename);
  uint64_t i = 0x12345678;
  double d = 1234567e+89;
  table.append(FB(A, i, d));
  auto fb1 = table.get(0);
  table.replace(FB(B, -i, -d), 0);
  auto fb2 = table.get(0);

  ASSERT_STREQ(fb1.str, STR_16(A));
  ASSERT_EQ(fb1.i, i);
  ASSERT_EQ(fb1.d, d);

  ASSERT_STREQ(fb2.str, STR_16(B));
  ASSERT_EQ(fb2.i, -i);
  ASSERT_EQ(fb2.d, -d);
}

TEST_F(FileTest, GetFlagTest) {
  // Remove when required method will be implemented
  return;
  FixedTable<int> table(filename);
  table.append(0x12345678);
  ASSERT_NE(table.getFlag(0), Flag::REMOVED);
}

TEST_F(FileTest, SetGetFlagsTest) {
  // Remove when required method will be implemented
  return;
  FixedTable<int> table(filename);
  table.append(0x12345678);
  // this probably can break for some day
  // replace 0 for some default value
  ASSERT_EQ(table.getFlag(0), 0);
  table.setFlag(0, Flag::REMOVED);
  ASSERT_EQ(table.getFlag(0), Flag::REMOVED);
}

TEST_F(FileTest, RemoveTest) {
  // Remove when required method will be implemented
  return;
  FixedTable<int> table(filename);
  table.append(0x12345678);
  ASSERT_NE(table.getFlag(0), Flag::REMOVED);
  table.remove(0);
  ASSERT_EQ(table.getFlag(0), Flag::REMOVED);
}

}
}
