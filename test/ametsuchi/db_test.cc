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

namespace ametsuchi {

using pager::FileHeader;
using pager::default_file_header;

const std::string name = "_test_database.db";

#define CLEANUP (remove(name.c_str()))

TEST(DBTest, DatabaseCreation) {
    DB db(name);
    db.save({1});
    db.close();
    ASSERT_NE(access(name.c_str(), F_OK), -1);
    FILE *f = fopen(name.c_str(), "rb");
    FileHeader header;
    EXPECT_EQ(fread(&header, sizeof(FileHeader), 1, f), 1u);
    ASSERT_EQ(strncmp(reinterpret_cast<const char*>(&header), reinterpret_cast<const char*>(&default_file_header), sizeof(FileHeader)), 0);
    CLEANUP;
}

TEST(DBTest, DatabaseReopening) {
    FILE *f = fopen(name.c_str(), "wb+");
    fwrite(&default_file_header, sizeof(FileHeader), 1, f);
    fclose(f);

    DB db;
    db.open(name);
    db.save({1});
    db.close();
    CLEANUP;
}

}
