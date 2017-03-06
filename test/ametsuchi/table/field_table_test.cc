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

#include <ametsuchi/table/field_table.h>
#include <ametsuchi/file/file.h>

namespace ametsuchi{
namespace table{

class FieldTableTest : public ::testing::Test{
protected:

    virtual void TearDown() {
        remove(filename.c_str());
    }
    const std::string filename = "/tmp/test_db";
};

TEST_F(FieldTableTest, AppendTest) {
    {
        file::AppendableFile af(filename);
        file::SequentialFile sf(filename);
        FieldTable ft(sf, af);
    }
}

}
}