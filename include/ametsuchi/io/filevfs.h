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

#include <ametsuchi/io/vfs.h>
#include <ametsuchi/pager/page.h>
#include <ametsuchi/pager/cache.h>
#include <cstdio>

namespace ametsuchi {
namespace io {

enum { FileVFS_MAX_CACHE_SIZE = 256, };

using FileHandler = FILE*;

/*
* This class perform the real basic implementation of VFS
* All operations are performed via C-based i/o function
* todo?: respect max_file_size & WAL
*/
class FileVFS : public virtual VFS {
public:
    /*
    * @param file base name of the file to operate
    *        todo?: file format: %file%-%file_number%.db
    */
    FileVFS(const char *file);
    virtual ~FileVFS();

    /*
    * Flushes the cached data and close active connection if present
    */
    virtual void close();

    /*
    * Perform reading data at specified file offset
    * @param ptr offset at the file in bytes
    * @param b array for storing the data
    * @param size number of bytes to read
    */
    virtual void read(std::size_t offset, ByteArray &b, std::size_t size);

    /*
    * Perform writing data at specified file offset of entire buffer
    * @param ptr offset at the file in bytes
    * @param b array for writing
    */
    virtual void write(std::size_t offset, const ByteArray &b);

    /*
    * Flushes dirty pages from cache to used FS
    */
    virtual void flush();
private:
    FileHandler f;
    Cache<std::size_t, pager::Page> cache;
};

}
}
