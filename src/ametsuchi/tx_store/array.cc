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

#include <ametsuchi/tx_store/array.h>
#include <ametsuchi/serializer.h>

namespace ametsuchi {
namespace tx_store {

Array::Array(const std::string &path)
  : file_(path){
  file_.open();
}

file::offset_t Array::append(const ByteArray &data) {
  ByteArray buffer(serialize::size(data));
  auto buffer_ptr = buffer.data();
  serialize::put(buffer_ptr, data);
  return file_.append(buffer);
}

ByteArray Array::get(const file::offset_t offset) {
  file_.seek(offset);
  size_t size;
  auto buffer = file_.read(serialize::size(size));
  const byte_t *buffer_ptr = buffer.data();
  serialize::get(&size, buffer_ptr);
  return file_.read(size);
}

}  // namespace tx_store
}  // namespace ametsuchi
