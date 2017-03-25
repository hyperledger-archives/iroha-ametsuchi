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

#include <ametsuchi/env.h>
#include <spdlog/spdlog.h>
//#include <sys/statvfs.h>
//#include <sys/sysinfo.h>

namespace ametsuchi {

static auto console = spdlog::stdout_color_mt("env");

uint64_t Env::get_available_memory() {
  // TODO cross-platform solution
  // detect amount of memory
  // http://stackoverflow.com/questions/63166/how-to-determine-cpu-and-memory-consumption-from-inside-a-process
//  struct sysinfo memInfo;
//  sysinfo(&memInfo);
  // to avoid overflow on right hand side
//  uint64_t mem = memInfo.totalram;
//  mem += memInfo.totalswap;
//  mem *= memInfo.mem_unit;
//  return mem;
  return 0;
}

std::string Env::get_database_directory() { return this->database_dir; }

std::string Env::get_logs_directory() { return this->logs_dir; }

std::string Env::get_index_directory() { return this->index_dir; }

void Env::set_database_directory(std::string path) {
  database_dir = mkdirectory(path);
}

void Env::set_logs_directory(std::string path) { logs_dir = mkdirectory(path); }

void Env::set_memory_limit(uint64_t limit) {
  uint64_t available = get_available_memory();
  if (limit > available) {
    console->critical("{} > {}", limit, available);
    throw Exception("set_memory_limit");
  }
}

Env& Env::get() {
  static Env e;
  console->info("default database dir is {}", e.database_dir);
  mkdirectory(e.database_dir);
  mkdirectory(e.logs_dir);
  mkdirectory(e.index_dir);
  return e;
}


std::string Env::mkdirectory(std::string path) {
  // TODO(@warchant) only POSIX now.
  // only user who created db can view its contents
  auto status = mkdir(path.c_str(), 0700);
  if (-1 == status) {
    console->critical("mkdir returned code {} for {}", errno, path);
    throw exception::IOError("mkdir");
  }

  console->debug("Directory {} created", path);

  // remove slashes in the end
  while (*path.rbegin() == '/') {
    path.pop_back();
  }

  return path + "/";
}

uint64_t Env::get_available_disk_space(const std::string& path) {
  // TODO cross-platform solution
  /*struct statvfs buf;
  auto status = statvfs(path.c_str(), &buf);
  if (-1 == status) {
    console->critical("statvfs returned code {} for {}", errno, path);
    throw exception::IOError("statvfs");
  }*/

  /*uint64_t disk_free = buf.f_bfree;
  disk_free *= buf.f_blocks;

  return disk_free;  // bytes*/
  return 0;
}
}
