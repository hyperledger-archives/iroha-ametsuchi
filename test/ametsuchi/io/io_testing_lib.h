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

#include <string>
#include <cstdio>

namespace ametsuchi {
namespace io {

const std::string name = "_test_database.db";

#define CLEANUP (remove(name.c_str()))

std::size_t CreateAndWrite(const char *bytes, std::size_t len) {
    FILE* f = fopen(name.c_str(), "wb+");
    if (!f) return 0;
    std::size_t written = fwrite(bytes, 1, len, f);
    fclose(f);
    return written;
}

std::size_t ReadFile(char *bytes, std::size_t len) {
    FILE *f = fopen(name.c_str(), "rb");
    if (!f) return 0;
    std::size_t written = fread(bytes, 1, len, f);
    fclose(f);
    return written;
}

}
}
