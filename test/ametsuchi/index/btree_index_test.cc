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
const int N = 500;

TEST(BTreeIndexTest, IntInsertTest) {
  {
    remove((dirname + "/data.mdb").c_str());
    remove((dirname + "/lock.mdb").c_str());
    rmdir(dirname.c_str());

    struct stat st = {0};


    if (stat(dirname.c_str(), &st) == -1) {
      mkdir(dirname.c_str(), 0775);
    }

    BTreeIndex<int, int> btree(dirname);
    std::vector<int> test_vals;
    for (int i = 0; i < N; i++) {
      test_vals.push_back(i);
      btree.insert(i, std::move(test_vals[i]));
    }

    for (int i = 0; i < N; ++i) {
      int val = btree.get(i);
      ASSERT_EQ(val, test_vals[i]);
    }
  }
}


TEST(BTreeIndexTest, IntGetTest) {
  {
    BTreeIndex<int, int> btree(dirname);
    for (int i = 0; i < N; ++i) {
      int val = btree.get(i);
      ASSERT_EQ(val, i);
    }
  }
}

TEST(BTreeIndexTest, ByteArrayInsertTest) {
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

TEST(BTreeIndexTest, BatchInsertTest) {
  {
    remove((dirname + "/data.mdb").c_str());
    remove((dirname + "/lock.mdb").c_str());
    rmdir(dirname.c_str());

    struct stat st = {0};


    if (stat(dirname.c_str(), &st) == -1) {
      mkdir(dirname.c_str(), 0775);
    }

    BTreeIndex<int, int> btree(dirname);
    std::vector<int> test_vals;
    std::vector<int> test_keys;
    for (int i = 0; i < N; i++) {
      test_keys.push_back(i);
      test_vals.push_back(i);
    }
    btree.insertBatch(test_keys, test_vals);

    for (int i = 0; i < N; i++) {
      int val = btree.get(i);
      ASSERT_EQ(val, test_vals[i]);
    }

  }
}


}
}
