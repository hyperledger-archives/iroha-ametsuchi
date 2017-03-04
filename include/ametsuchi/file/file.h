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

#ifndef AMETSUCHI_FILE_H
#define AMETSUCHI_FILE_H

#include <memory>
#include <string>
#include <exception>

namespace ametsuchi {
namespace file {

class File {
 public:
  explicit File(const std::string &path);

  virtual ~File() = 0;

 protected:
  std::string path_;
  std::unique_ptr<FILE, decltype(&fclose)> file_;
};

class SequentialFile : public File {
 public:
  SequentialFile(const std::string &path);

  template <typename T>
  void read(T *data, std::size_t size, std::size_t offset);

};

class AppendableFile : public File {
 public:
  AppendableFile(const std::string &path);

  template <typename T>
  void append(const T &data);

};

}  // namespace file
}  // namespace ametsuchi

#endif  // AMETSUCHI_FILE_H
