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
#include <ametsuchi/globals.h>

namespace ametsuchi {
namespace file {

using offset_t = uint64_t;

class File {
 public:
  explicit File(const std::string &path);

  offset_t position() const;

  virtual bool open() = 0;

  void close();

  virtual ~File() = 0;

 protected:
  std::string path_;
  std::unique_ptr<FILE, decltype(&std::fclose)> file_;
};

class SequentialFile : public File {
  using File::File;
 public:
  bool open() override;

  template <typename T>
  std::size_t read(T *data, std::size_t num, offset_t offset) {
    std::fseek(file_.get(), offset, SEEK_CUR);
    return std::fread(data, sizeof(T), num, file_.get());
  }

  template <typename T>
  std::size_t read(std::vector<T> data, std::size_t num, offset_t offset) {
    data.reserve(num);
    std::fseek(file_.get(), offset, SEEK_CUR);
    return std::fread(&data[0], sizeof(T), num, file_.get());
  }

  ByteArray read(std::size_t size, offset_t offset);
};

class AppendableFile : public File {
  using File::File;
 public:
  bool open() override;

  template <typename T>
  std::size_t append(const std::vector<T> &data) {
    auto res = std::fwrite(data.data(), sizeof(T), data.size(), file_.get());
    std::fflush(file_.get());
    return res;
  }

  template <typename T>
  std::size_t append(const T &data) {
    auto res = std::fwrite(data, sizeof(T), 1, file_.get());
    std::fflush(file_.get());
    return res;
  }
};

}  // namespace file
}  // namespace ametsuchi

#endif  // AMETSUCHI_FILE_H
