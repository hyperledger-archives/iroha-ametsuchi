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

#include <ametsuchi/db.h>
#include <ametsuchi/pager/page.h>
#include <ametsuchi/globals.h>
#include <string.h>
#include "io/io_testing_lib.h"

namespace ametsuchi {

using pager::FileHeader;
using pager::default_file_header;
using io::name;

TEST(DBTest, DatabaseCreation) {
    DB db(name);
    db.save({1});
    db.close();
    ASSERT_NE(access(name.c_str(), F_OK), -1);
    FileHeader header;
    EXPECT_EQ(io::ReadFile((char*)(&header), sizeof(FileHeader)), 16u);
    ASSERT_EQ(strncmp(reinterpret_cast<const char*>(&header), reinterpret_cast<const char*>(&default_file_header), sizeof(FileHeader)), 0);
    CLEANUP;
}

TEST(DBTest, DatabaseReopening) {
    io::CreateAndWrite((const char*)(&default_file_header), sizeof(FileHeader));
    DB db;
    db.open(name);
    db.save({1});
    db.close();
    CLEANUP;
}

}
