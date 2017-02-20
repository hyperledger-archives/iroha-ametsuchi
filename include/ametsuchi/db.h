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

#pragma once

#include <ametsuchi/globals.h>
#include <ametsuchi/io/vfs.h>

namespace ametsuchi {

class DB {
 public:
    /*
    * Empty constructor
    */
    DB();
    /*
    * Constructor that creates new file,
    * filling the header and first page header (not finished)
    */
    DB(const std::string &file_path);

    ~DB();

    /*
    * Opens file by specified name. Close active
    * VFS handler if there's any
    * @param file_path name of the file to open
    */
    void open(const std::string &file_path);

    /*
    * Append data to the database.
    * Don't check for validity VFS handler
    * @param tx data to write
    */
    void save(const ByteArray &tx);

    /*
    * Safely close all opened resources
    */
    void close();
 private:
    DB(std::unique_ptr<io::VFS>&& vfs);

    /*
    * VFS handler of database
    */
    std::unique_ptr<io::VFS> vfs;

    /*
    * Size of the database
    */
    size_t size;
};

}
