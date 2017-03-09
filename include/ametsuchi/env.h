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
#include <spdlog/spdlog.h>
#include <sys/stat.h>
#include <sys/sysinfo.h>
#include <string>

namespace ametsuchi {

using exception::Exception;

static auto console = spdlog::stdout_color_mt("env");

/**
 * This structure stores current working environment and global per-db settings.
 */
// TODO(@warchant): extract methods to cc file, leave only interface here.
class Env {
 public:
  Env() : memory_limit(get_available_memory()) {}

  std::string get_database_directory() { return this->database_directory; }

  std::string get_logs_directory() { return this->logs_directory; }

  std::string get_index_directory() { return this->index_directory; }

  void set_database_directory(std::string path) {
    database_directory = mkdirectory(path);
  }

  void set_logs_directory(std::string path) {
    logs_directory = mkdirectory(path);
  }

  void set_memory_limit(uint64_t limit) {
    uint64_t available = get_available_memory();
    if (limit > available) {
      console->critical("{} > {}", limit, available);
      throw Exception("set_memory_limit");
    }
  }

 private:
  // by default memory_limit is set to max available memory
  uint64_t memory_limit;  // bytes

  std::string database_directory;
  std::string logs_directory;
  std::string index_directory;

  // in bytes
  uint64_t get_available_memory() {
    // detect amount of memory
    // http://stackoverflow.com/questions/63166/how-to-determine-cpu-and-memory-consumption-from-inside-a-process
    sysinfo memInfo;
    sysinfo(&memInfo);
    // to avoid overflow on right hand side
    uint64_t mem = memInfo.totalram;
    mem += memInfo.totalswap;
    mem *= memInfo.mem_unit;
    return mem;
  }

  std::string mkdirectory(std::string path) {
    // TODO(@warchant) only POSIX now.
    // only user who created db can view its contents
    auto status = mkdir(path.c_str(), 0700);
    if (-1 == status) {
      switch (errno) {
        case EEXIST: {
          console->critical("The named file exists: {}", path);
          throw Exception("mkdirectory::EEXIST");
        }

        case EACCES: {
          console->critical(
              "Search permission is denied on a component of the path prefix, "
              "or "
              "write permission is denied on the parent directory of the "
              "directory to be created: {0}",
              path);
          throw Exception("mkdirectory::EACCES");
        }

        case ELOOP: {
          console->critical(
              "A loop exists in symbolic links encountered during resolution "
              "of "
              "the path argument: {}",
              path);
          throw Exception("mkdirectory::ELOOP");
        }

        case EMLINK: {
          console->critical(
              "The link count of the parent directory would exceed LINK_MAX: "
              "{}",
              path);
          throw Exception("mkdirectory::EMLINK");
        }

        case ENAMETOOLONG: {
          console->critical(
              "The length of the path argument exceeds PATH_MAX or a pathname "
              "component is longer than NAME_MAX: {}",
              path);
          throw Exception("mkdirectory::ENAMETOOLONG");
        }

        case ENOENT: {
          console->critical(
              "A component of the path prefix specified by path does not name "
              "an "
              "existing directory or path is an empty string: {}",
              path);
          throw Exception("mkdirectory::ENOENT");
        }

        case ENOSPC: {
          console->critical(
              "The file system does not contain enough space to hold the "
              "contents of the new directory or to extend the parent directory "
              "of the new directory: {}",
              path);
          throw Exception("mkdirectory::ENOSPC");
        }

        case ENOTDIR: {
          console->critical(
              "A component of the path prefix is not a directory: {}", path);
          throw Exception("mkdirectory::ENOTDIR");
        }

        case EROFS: {
          console->critical(
              "The parent directory resides on a read-only file system: {}",
              path);
          throw Exception("mkdirectory::EROFS");
        }
        default:
          console->critical("Some unexpected error: {}", path);
          throw Exception("mkdirectory::unexpected");
      }
    }

    console->debug("Directory {} created", path);

    // remove slashes in the end
    while (*path.rend() == '/') {
      path.pop_back();
    }

    return path + "/";
  }
};
}  // namespace ametsuchi

#endif  // AMETSUCHI_ENV_H
