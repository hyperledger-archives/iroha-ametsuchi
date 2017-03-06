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

#include <ametsuchi/table/fixed_table.h>
#include <ametsuchi/globals.h>

namespace ametsuchi{
namespace table{

class FileTest : public ::testing::Test{
 protected:

  virtual void TearDown() {
    remove(filename.c_str());
  }

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
#define STR_16(c) #c#c#c#c\
                  #c#c#c#c\
                  #c#c#c#c\
                  #c#c#c

TEST_F(FileTest, AppendGetTest) {
  FixedTable<ExampleFB> table(filename);
  uint64_t i = 0x12345678;
  double d = 1234567e+89;
  table.append(ExampleFB{STR_16(A), i, d,});
  auto fb = table.get(0);

  ASSERT_STREQ(fb.str, STR_16(A));
  ASSERT_EQ(fb.i, i);
  ASSERT_EQ(fb.d, d);
}

}
}
