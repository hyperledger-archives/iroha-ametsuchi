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

class FileTest : public ::testing::Test{
 protected:

  virtual void TearDown() {
    remove(filename.c_str());
  }

  const std::string filename = "/tmp/test_db";
};

TEST_F(FileTest, AppendTest) {
  {
    AppendableFile af(filename);
    af.open();

    ByteArray wdata = {1, 2, 3};
    af.append(wdata);

    wdata = {3, 2, 1};
    af.append(wdata);
  }

  {
    SequentialFile sf(filename);
    sf.open();

    ByteArray wdata = {1, 2, 3, 3, 2, 1};

    ByteArray rdata(6);
    sf.read(rdata.data(), rdata.size(), 0);

    ASSERT_EQ(wdata, rdata);
  }

  {
    SequentialFile sf(filename);
    sf.open();

    ByteArray wdata = {1, 2, 3, 3, 2, 1};

    ByteArray rdata = sf.read(wdata.size(), 0);

    ASSERT_EQ(wdata, rdata);
  }

}

TEST_F(FileTest, PositionTest){
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

TEST_F(FileTest, SimultaneousReadWrite){
  AppendableFile af(filename);
  SequentialFile sf(filename);

  af.open();
  sf.open();

  af.append(ByteArray{1, 2, 3});
  ASSERT_EQ(sf.read(2, 0), ByteArray({1, 2}));

  af.append(ByteArray{4, 5, 6});
  ASSERT_EQ(sf.read(2, 2), ByteArray({5, 6}));
}

TEST_F(FileTest, NonexistantFile){
  SequentialFile sf(filename);
  ASSERT_EQ(sf.open(), false);
}

TEST_F(FileTest, RandomAccessTest){
  {
    AppendableFile af(filename);
    af.open();

    af.append(ByteArray{1, 2, 3, 4, 5, 6, 7, 8, 9, 10});
  }
  RandomAccessFile raf(filename);
  raf.open();

  ASSERT_EQ(raf.read(2, 0), ByteArray({1, 2}));

  ASSERT_EQ(raf.read(2, 2), ByteArray({3, 4}));

  ASSERT_EQ(raf.read(2, 3), ByteArray({4, 5}));

  ASSERT_EQ(raf.read(5, 9), ByteArray({10}));
}
}
}
