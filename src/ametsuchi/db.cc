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
#include <iostream>

namespace ametsuchi {

DB::DB(const std::string &file_path) {
    // TODO: there should be handling configuration params as well

    file.open(file_path);

    file<<"天地\x00";
}

DB::~DB() {
    close();
}

std::unique_ptr<DB> DB::open(const std::string &file_path) {
    std::fstream file;
    file.open(file_path, std::ios::in | std::ios::out | std::ios::binary);
    std::unique_ptr<DB> db(new DB(file));
    return db;
}

template<size_t T>
void DB::save(const ByteArray<T> &tx) {

}

void DB::close() {
    file.close();
}

DB::DB(std::fstream &f) {
    file = std::move(f);
}

}
