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

#pragma once

#include <ametsuchi/globals.h>

namespace ametsuchi {
namespace io {

/*
* Interface for performing all I/O operations for the db
* This class assumes that it working with one file
*/
class VFS {
 public:
    virtual ~VFS() = 0;

    /*
    * Flushes the cached data and close active connection if present
    */
    virtual void close() = 0;

    /*
    * Perform reading data at specified file offset
    * @param ptr offset at the file in bytes
    * @param b array for storing the data
    * @param size number of bytes to read
    */
    virtual void read(std::size_t ptr, ByteArray &b, std::size_t size) = 0;

    /*
    * Perform writing data at specified file offset of entire buffer
    * @param ptr offset at the file in bytes
    * @param b array for writing
    */
    virtual void write(std::size_t ptr, const ByteArray &b) = 0;

    /*
    * Flushes dirty pages from cache to used FS
    */
    virtual void flush() = 0;
};

}
}
