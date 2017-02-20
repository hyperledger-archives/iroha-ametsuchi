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

#include <ametsuchi/db.h>
#include <ametsuchi/pager/page.h>
#include <ametsuchi/io/cvfs.h>
#include <iostream>

namespace ametsuchi {

DB::DB() : vfs(nullptr) {}

DB::DB(const std::string &file_path) {
    // TODO: there should be handling configuration params as well
    open(file_path);

    // definitely not so good serialization
    using pager::default_file_header;
    auto ptr = reinterpret_cast<const uint8_t*>(&default_file_header);
    ByteArray arr(ptr, ptr + sizeof default_file_header);
    save(arr);
}

DB::~DB() {
    close();
}

void DB::open(const std::string &file_path/*, todo: VFSType type*/) {
    if (!vfs) {
        close();
    }

    vfs = std::make_unique<io::CVFS>(file_path.c_str());
}

void DB::save(const ByteArray &tx) {
    vfs->write(size, tx);
    size += tx.size();
}

void DB::close() {
    size = 0;
    if (!vfs) return;
    vfs->close();
}

DB::DB(std::unique_ptr<io::VFS>&& vfs) : vfs(std::move(vfs)) {}

}
