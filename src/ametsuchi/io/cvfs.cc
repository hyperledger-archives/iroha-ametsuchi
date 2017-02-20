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

namespace ametsuchi {
namespace io {

CVFS::CVFS(FileHandler &f) : f(f) {}

CVFS::~CVFS() {
    close();
}

void CVFS::close() {
    fclose(f);
}

void CVFS::read(size_t ptr, Byte &b, size_t size) {
    fseek(f, ptr, SEEK_SET);
    fread(b, size, 1, f);
}

void CVFS::write(size_t ptr, const Byte &b, size_t size) {
    fseek(f, ptr, SEEK_SET);
    fwrite(b, size, 1, f);
}


}
}
