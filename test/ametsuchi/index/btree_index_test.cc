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

#include <ametsuchi/index/btree_index.h>

#include <ametsuchi/file/file.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>


namespace ametsuchi {
namespace index {

class BTreeIndexTest : public ::testing::Test {
 protected:
  virtual void TearDown() {
    /*
    remove((dirname + "/data.mdb").c_str());
    remove((dirname + "/lock.mdb").c_str());
    rmdir(dirname.c_str());
     */
  }
  const std::string dirname = "/tmp/test_dir";
};

const std::string dirname = "/tmp/test_dir";


TEST(BTreeIndexTest, SimpleInsertTest) {
  {
    remove((dirname + "/data.mdb").c_str());
    remove((dirname + "/lock.mdb").c_str());
    rmdir(dirname.c_str());

    struct stat st = {0};

    if (stat(dirname.c_str(), &st) == -1) {
      mkdir(dirname.c_str(), 0775);
    }

    BTreeIndex<int, int> btree(dirname);
    std::vector<int> test_vals = {10, 20, 30, 40, 50};
    for (int i = 0; i < test_vals.size(); i++) {
      btree.insert(i, std::move(test_vals[i]));
    }

    test_vals = {10, 20, 30, 40, 50};
    for (int i = 0; i < test_vals.size(); ++i) {
      int val = btree.get(i);
      ASSERT_EQ(val, test_vals[i]);
    }
    printf("Simple insert test \n");
  }
}


TEST(BTreeIndexTest, IntGetTest) {
  {
    BTreeIndex<int, int> btree(dirname);

    ASSERT_EQ(btree.get(0), 10);
    ASSERT_EQ(btree.get(1), 20);
  }
}



TEST(BTreeIndexTest, ByteArrayTest) {
  {
    remove((dirname + "/data.mdb").c_str());
    remove((dirname + "/lock.mdb").c_str());
    rmdir(dirname.c_str());
    struct stat st = {0};

    if (stat(dirname.c_str(), &st) == -1) {

      mkdir(dirname.c_str(), 0775);
    }

    BTreeIndex<ametsuchi::file::offset_t, ByteArray> btree(dirname);

    ByteArray wdata1 = {4, 3, 2, 1};
    ByteArray wdata2 = {5, 6, 7, 8};
    ametsuchi::file::offset_t a = 0;

    btree.insert(a, std::move(wdata1));
    a = 1;
    btree.insert(1, std::move(wdata2));

    ASSERT_EQ(0, 0);


  }
}



TEST(BTreeIndexTest, ByteArrayGetTest) {
  {
    struct stat st = {0};

    if (stat(dirname.c_str(), &st) == -1) {
      printf("Created ! ");
      mkdir(dirname.c_str(), 0775);
    }

    BTreeIndex<ametsuchi::file::offset_t, ByteArray> btree(dirname);

    ByteArray wdata1 = {4, 3, 2, 1};
    ByteArray wdata2 = {5, 6, 7, 8};

    ametsuchi::file::offset_t a = 0;
    ASSERT_EQ(btree.get(a), wdata1);
    a = 1;
    ASSERT_EQ(btree.get(a), wdata2);

  }
}


}
}
