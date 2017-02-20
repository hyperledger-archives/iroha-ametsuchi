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

#include <ametsuchi/io/cvfs.h>
#include <cstring>

namespace ametsuchi {
namespace io {

FileVFS::FileVFS(const char *file) : cache(FileVFS_MAX_CACHE_SIZE) {
    f = fopen(file, "r+"); // maybe a+?
}

FileVFS::~FileVFS() {
    close();
}

void FileVFS::close() {
    flush();
    fclose(f);
}

void FileVFS::read(std::size_t offset, ByteArray &b, std::size_t size) {
    size_t page_size = pager::AMETSUCHI_PAGE_SIZE;
    auto base_page = pager::page_idx(offset);
    auto page_num = pager::page_ceil(size);
    b.clear();
    b.reserve(size);

    pager::Page *p;
    for (size_t i = 0; i < page_num; ++i) {
        pager::Page* pg = cache.get(base_page);

        // TODO: store missed pages at read from disk at once
        if (!pg) {
            fseek(f, offset, SEEK_SET);
            fread(p, page_size, 1, f);
        } else {
            p = pg;
        }

        for (uint8_t i : p->page){
            b.push_back(i);
        }
    }

}

void FileVFS::write(std::size_t offset, const ByteArray &b) {
    size_t page_size = pager::AMETSUCHI_PAGE_SIZE;
    auto base_page = pager::page_idx(offset);
    auto size = b.size();
    auto page_num = pager::page_ceil(size);

    pager::Page p;
    size_t not_filled = (0 != size % page_size) ? 1 : 0;
    for (size_t i = 0; i < (page_num - not_filled) ; ++i) {
        memcpy(p.page, b.data() + (page_size * i), page_size);
        cache.put(base_page + i, std::move(p));
    }

    fseek(f, offset, SEEK_SET);
    fwrite(b.data(), b.size(), 1, f);
}

void FileVFS::flush() {
    //TODO: consider dirty pages
    fflush(f);
    cache.clear();
}

}
}
