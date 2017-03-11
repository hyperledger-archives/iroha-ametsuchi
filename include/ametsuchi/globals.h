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

#ifndef AMETSUCHI_GLOBALS_H_
#define AMETSUCHI_GLOBALS_H_

#include <ametsuchi/env.h>
#include <ametsuchi/exception.h>
#include <cstdint>
#include <cstdlib>
#include <cassert>
#include <memory>
#include <string>
#include <vector>

namespace ametsuchi {

using byte_t = uint8_t;
using ByteArray = std::vector<byte_t>;

}

#endif  // AMETSUCHI_GLOBALS_H_
