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

const std::string filename = "test";
/*
TEST(FileTest, WriteTest) {
  WritableFile wf(filename);

  ByteArray wdata = {1, 2, 3};
  wf.write(wdata);

  ByteArray rdata(3);
  wf.read(rdata, 0);

  ASSERT_EQ(wdata, rdata);

  wdata = {3, 2, 1};
  wf.write(wdata);

  wf.read(rdata, 0);

  ASSERT_EQ(wdata, rdata);

  remove(filename.c_str());
}*/

TEST(FileTest, AppendTest) {
  {
    AppendableFile af(filename);

    ByteArray wdata = {1, 2, 3};
    af.append(wdata);

    wdata = {3, 2, 1};
    af.append(wdata);
  }
  {
    SequentialFile sf(filename);

    ByteArray wdata = {1, 2, 3, 3, 2, 1};
    ByteArray rdata(6);
    sf.read(rdata.data(), rdata.size(), 0);
    ASSERT_EQ(wdata, rdata);
  }

  remove(filename.c_str());
}
}
}
