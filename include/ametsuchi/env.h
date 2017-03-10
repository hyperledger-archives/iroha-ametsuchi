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

#ifndef AMETSUCHI_ENV_H
#define AMETSUCHI_ENV_H

#include <ametsuchi/exception.h>
#include <memory>
#include <string>

namespace ametsuchi {

using exception::Exception;


/**
 * This structure stores current working environment and global per-db settings.
 */
class Env {
 public:
  Env()
      : memory_limit(get_available_memory()),
        database_dir("/tmp/ametsuchi"),
        logs_dir(database_dir + "/logs"),
        index_dir(database_dir + "/index") {}

  std::string get_database_directory();

  std::string get_logs_directory();

  std::string get_index_directory();

  void set_database_directory(std::string path);

  void set_logs_directory(std::string path);

  void set_memory_limit(uint64_t limit);

  static std::string mkdirectory(std::string path);

  static uint64_t get_available_memory();

  static uint64_t get_available_disk_space(const std::string& path);

  static Env& get();

 private:
  // by default memory_limit is set to max available memory
  uint64_t memory_limit;  // bytes

  std::string database_dir;
  std::string logs_dir;
  std::string index_dir;

  static std::shared_ptr<Env> ENV;
};
}  // namespace ametsuchi

#endif  // AMETSUCHI_ENV_H
