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

  virtual void TearDown() {
    remove(filename.c_str());
  }

  const std::string filename = "/tmp/test_db";
};

struct PACKED_STRUCT ExampleFB {
  char str[16];
  uint64_t i;
  double d;
};

// 15 char s + ending \0
#define STR_16(c) #c#c#c#c\
                  #c#c#c#c\
                  #c#c#c#c\
                  #c#c#c
#define FB(c, i, d) ExampleFB{STR_16(c), i, d}
#define ASSERT_FBEQ(fb1, fb2)\
    ASSERT_STREQ((fb1).str, (fb2).str);\
    ASSERT_EQ((fb1).i, (fb2).i);\
    ASSERT_EQ((fb1).d, (fb2).d)
#define ASSERT_FBEQ_EMP(fb, c, i, d)\
    ASSERT_STREQ((fb).str, STR_16(c));\
    ASSERT_EQ((fb).i, i);\
    ASSERT_EQ((fb).d, d)

TEST_F(FileTest, AppendGetTest) {
  FixedTable<ExampleFB> table(filename);
  uint64_t i = 0x12345678;
  double d = 1234567e+89;
  auto idx = table.append(FB(A, i, d));
  auto fb = table.get(idx);

  ASSERT_FBEQ_EMP(fb, A, i, d);
}

TEST_F(FileTest, AppendGetBatchTest) {
  FixedTable<ExampleFB> table(filename);
  uint64_t i[] = {0x12345678, 0x87654321, 0xfedcba9876543210, 0x123456789abcdef};
  double d[] = {1234567e+89, 9876543e+21, 0xfedcba9876543, 0x123456789abcde};
  std::vector<ExampleFB> base_fbs = {
    FB(A, i[0], d[0]),
    FB(B, i[1], d[1]),
    FB(C, i[2], d[2]),
    FB(D, i[3], d[3])
  };
  auto idx = table.appendBatch(base_fbs);
  auto received_fbs = table.getBatch(base_fbs.size(), idx);

  ASSERT_EQ(received_fbs.size(), base_fbs.size());

  auto it1 = base_fbs.begin();
  auto it2 = received_fbs.begin();
  for (; it1 != base_fbs.end(); ++it1, ++it2) {
    ASSERT_FBEQ(*it1, *it2);
  }
}

TEST_F(FileTest, AppendGetBatchBigTest) {
  struct PACKED_STRUCT S {
    char c;
    int i;
  };
  constexpr size_t num = 4096;
  FixedTable<S> table(filename);
  S s{0x42, 0x13579bdf};
  file::offset_t idx = table.appendBatch(std::vector<S>(num, s));
  auto received = table.getBatch(num, idx);
  for (auto r : received) {
    ASSERT_EQ(r.i, s.i);
    ASSERT_EQ(r.c, s.c);
  }
}

TEST_F(FileTest, AppendReplaceGetTest) {
  FixedTable<ExampleFB> table(filename);
  uint64_t i = 0x12345678;
  double d = 1234567e+89;
  auto idx = table.append(FB(A, i, d));
  auto fb1 = table.get(idx);
  table.replace(FB(B, ~i, -d), idx);
  auto fb2 = table.get(idx);

  ASSERT_FBEQ_EMP(fb1, A, i, d);
  i = ~i; d = -d;
  ASSERT_FBEQ_EMP(fb2, B, i, d);
}

TEST_F(FileTest, GetFlagTest) {
  FixedTable<int> table(filename);
  table.append(0x12345678);
  ASSERT_NE(table.getFlag(0), Flag::REMOVED);
}

TEST_F(FileTest, SetGetFlagsBatchTest) {
  constexpr size_t num = 4096;
  FixedTable<int> table(filename);
  file::offset_t idx = table.appendBatch(std::vector<int>(num, 0x12345678));
  for (int i = 0; i < num; ++i) {
    ASSERT_EQ(table.getFlag(idx + i), Flag::VALID);
  }

  for (int i = 0; i < num; i += 2) {
    table.setFlag(i, Flag::REMOVED);
  }
  for (int i = 0; i < num; ++i) {
    ASSERT_EQ(table.getFlag(idx + i), (i % 2 == 0 ? Flag::REMOVED : Flag::VALID));
  }
}

TEST_F(FileTest, RemoveTest) {
  FixedTable<int> table(filename);
  table.append(0x12345678);
  ASSERT_NE(table.getFlag(0), Flag::REMOVED);
  table.remove(0);
  ASSERT_EQ(table.getFlag(0), Flag::REMOVED);
}

}
}
