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

#include "ametsuchi/table/FieldTable.h"

namespace ametsuchi {
namespace table {

//FieldTable::FieldTable(const file::File &file1) : file_(file1){}
FieldTable::FieldTable(const std::string &path) : path_(path), separator('@') {};

bool FieldTable::put(const ByteArray &value) {
    std::unique_ptr<file::AppendableFile> file_ = std::unique_ptr<file::AppendableFile>(new file::AppendableFile(path_));
    ByteArray memory(4+value.size()); // assume size of flatbuffer would fit to 4 bytes

    uint32_t size = value.size();
    uint8_t* ptr = memory.data();

    PUT_UINT(ptr, size, uint32_t);
    PUT_BYTE_ARRAY(ptr, value);

    file_->append(memory);
    return true;
}

ByteArray &FieldTable::get(const offset_t offset) {
    std::unique_ptr<file::SequentialFile> file_ = std::unique_ptr<file::AppendableFile>(new file::SequentialFile(path_));

    ByteArray lengthBytes = file_->read(sizeof(uint32_t), offset);
    uint32_t length = 0;

    GET_UINT(&length, lengthBytes.data(), uint32_t);
    ByteArray value = file_->read(length, offset+sizeof(uint32_t));
    return value;
}


}
}