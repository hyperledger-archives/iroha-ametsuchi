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

#ifndef AMETSUCHI_DISK_ARRAY_H
#define AMETSUCHI_DISK_ARRAY_H

#include <memory>
#include <ametsuchi/globals.h>
#include <ametsuchi/file/file.h>
#include "table.h"

namespace ametsuchi{
    namespace table {

        using offset_t = uint64_t;

        class FieldTable {
        public:
            explicit FieldTable(const std::string &path);
            bool put(const ByteArray &value);
            ByteArray &get(const offset_t);

            class ForwardIterator;

        private:
            std::unique_ptr<file::File> file_;
            std::string path_;
            char separator;
        };

        class FieldTable::ForwardIterator {
        public:
            ForwardIterator();
            ~ForwardIterator();
            ForwardIterator(const ForwardIterator &it);
            ForwardIterator(const ForwardIterator &&it);
            void operator=(const ForwardIterator &r);    // =
            void operator=(const ForwardIterator &&r);   // =
            ForwardIterator &operator++();               // postfix++
            ForwardIterator &operator++(int);            // ++prefix
            ByteArray &operator*();                  // dereference
            ByteArray &operator->();                 // dereference
            bool operator==(const ForwardIterator &it);  // ==
            bool operator<(const ForwardIterator &it);   // <
            bool operator>(const ForwardIterator &it);   // >
        protected:
            offset_t offset;
        };

    }
}

#endif //AMETSUCHI_DISK_ARRAY_H
