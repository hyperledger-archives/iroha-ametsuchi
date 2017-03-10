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
#include <ametsuchi/table/table.h>
#include <ametsuchi/globals.h>

namespace ametsuchi {
namespace table {

class FileTestHelper : public ::testing::Test {
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


constexpr size_t num = 4096;

}
}
