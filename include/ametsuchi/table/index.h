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

#ifndef AMETSUCHI_INDEX_H
#define AMETSUCHI_INDEX_H

#include <ametsuchi/file/file.h>
#include <utility>
#include <map>

namespace ametsuchi {
namespace table {

using ametsuchi::file::offset_t;

/**
 * In-memory index (temporarily).
 *
 * Should be changed to data structure, which is able to store whole index on
 * disk and only "hot" index in-memory.
 */
using Index = std::map<ByteArray, offset_t>;

}  // namespace table
}  // namespace ametsuchi

#endif  // AMETSUCHI_INDEX_H
