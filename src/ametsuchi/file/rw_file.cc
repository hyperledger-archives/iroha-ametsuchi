/**
 * Copyright Soramitsu Co., Ltd. 2017 All Rights Reserved.
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

#include <ametsuchi/file/rw_file.h>

namespace ametsuchi {
namespace file {


ReadWriteFile::ReadWriteFile(const std::string &path) : File(path) {
  read_ = true;
  write_ = true;
}

bool ReadWriteFile::open() {
  file_.reset(std::fopen(path_.c_str(), "r+b"));
  if (!file_.get()) {
    file_.reset(std::fopen(path_.c_str(), "w+b"));
  }
  // to read statistics
  bool opened = File::open();
  return !!file_ && opened;
}
}
}
