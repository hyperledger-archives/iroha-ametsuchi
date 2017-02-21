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

#include <ametsuchi/io/filevfs.h>
#include <ametsuchi/io/vfs.h>
#include <ametsuchi/globals.h>
#include <memory>
#include "io_testing_lib.h"

namespace ametsuchi {
namespace io {


TEST(FileVFSTest, Reading) {
    const uint8_t i[] = {4, 8, 15, 16, 23, 42};
    const std::size_t size = sizeof(i);
    const std::size_t bytes = size * sizeof(*i);
    ASSERT_EQ(CreateAndWrite((const char*)i, bytes), bytes);

    ByteArray buffer;
    auto vfs = std::make_unique<FileVFS>(name.c_str());
    vfs->read(0, buffer, bytes);
    vfs->close();

    // If vector smaller than we didn't read enough bytes
    ASSERT_GE(buffer.size(), size);
    for (std::size_t j = 0; j < size; ++j) {
        ASSERT_EQ(buffer[j], i[j]);
    }
    CLEANUP;
}

TEST(FileVFSTest, Writing) {
    ByteArray buffer = {4, 8, 15, 16, 23, 42};
    std::size_t size = buffer.size();
    auto vfs = std::make_unique<FileVFS>(name.c_str());
    vfs->write(0, buffer);
    vfs->close();

    uint8_t buf[size];
    ASSERT_EQ(ReadFile((char*)buf, size), size);

    for (std::size_t j = 0; j < size; ++j) {
        ASSERT_EQ(buffer[j], buf[j]);
    }
    CLEANUP;
}

}
}
