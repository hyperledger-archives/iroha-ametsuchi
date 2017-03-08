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
        FieldTable ft(filename);

        ByteArray wdata1 = {4, 3, 2, 1};
        file::offset_t offset1 = ft.append(wdata1);

        ByteArray wdata2 = {5, 6, 7, 8};
        file::offset_t offset2 = ft.append(wdata2);

        ByteArray rdata1 = ft.get(offset1);
        ASSERT_EQ(wdata1, rdata1);

        ByteArray rdata2 = ft.get(offset2);
        ASSERT_EQ(wdata2, rdata2);

    }
}

TEST_F(FieldTableTest, IteratorTest) {
    {
        FieldTable ft(filename);

        ByteArray wdata1 = {4, 3, 2, 1};
        file::offset_t offset1 = ft.append(wdata1);

        ByteArray wdata2 = {5, 6, 7, 8};
        file::offset_t offset2 = ft.append(wdata2);

        ByteArray rdata1 = ft.get(offset1);
        ASSERT_EQ(wdata1, rdata1);

        ByteArray rdata2 = ft.get(offset2);
        ASSERT_EQ(wdata2, rdata2);

        int count = 0;
        for (auto iter = ft.begin(); iter < ft.end(); iter++){
            if (count == 0)
                ASSERT_EQ(wdata1, *iter);
            else if (count == 1) {
                ASSERT_EQ(wdata2, *iter);
                ASSERT_TRUE(++iter == ft.end());
            }
            count++;
        }


    }
}

}
}
