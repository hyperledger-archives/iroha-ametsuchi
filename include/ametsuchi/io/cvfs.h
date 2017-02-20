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
#include <ametsuchi/cache.h>
#include <cstdio>

namespace ametsuchi {
namespace io {

enum { CVFS_MAX_FILE_SIZE = 4096 * 16, CVFS_MAX_CACHE_SIZE = 256, };

typedef FILE* FileHandler;

class CVFS : public virtual VFS {
public:
    CVFS(const char *file);
    virtual ~CVFS();
    virtual void close();
    virtual void read(std::size_t ptr, ByteArray&, std::size_t size);
    virtual void write(std::size_t ptr, const ByteArray&);
    virtual void fflush();
private:
    FileHandler f;
    Cache<std::size_t, pager::Page> cache;
};

}
}
