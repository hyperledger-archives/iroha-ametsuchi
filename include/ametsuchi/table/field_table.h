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

#ifndef AMETSUCHI_FIELD_TABLE_H
#define AMETSUCHI_FIELD_TABLE_H

#include <memory>
#include <ametsuchi/globals.h>
#include <ametsuchi/file/file.h>
#include <ametsuchi/table/table.h>

namespace ametsuchi{
    namespace table {

        using offset_t = uint64_t;

        class FieldTable {
        public:
            class ForwardIterator;

            explicit FieldTable(file::SequentialFile& seqFile, file::AppendableFile& appFile);
            offset_t put(const ByteArray &value);
            ByteArray get(const offset_t);
            ForwardIterator begin();
            ForwardIterator end();

        private:
            file::SequentialFile& seqFile_;
            file::AppendableFile& appFile_;
            std::string path_;
            offset_t fileLength;

            static const size_t BUF_SIZE = 1024;
        };

        class FieldTable::ForwardIterator {
        public:
            ForwardIterator(FieldTable& ft);
            ForwardIterator(FieldTable& ft, offset_t offset);
//            ~ForwardIterator();
            ForwardIterator(const ForwardIterator &it);
            ForwardIterator(const ForwardIterator &&it);
            void operator=(const ForwardIterator &r);    // =
            void operator=(const ForwardIterator &&r);   // =
            ForwardIterator &operator++();               // postfix++
            ForwardIterator operator++(int);            // ++prefix
            ByteArray &operator*();                  // dereference
            ByteArray &operator->();                 // dereference
            bool operator==(const ForwardIterator &it);  // ==
            bool operator<(const ForwardIterator &it);   // <
            bool operator>(const ForwardIterator &it);   // >
        protected:
            FieldTable& ft_;
            offset_t offset_;
            ByteArray value_;
        };

    }
}

#endif //AMETSUCHI_FIELD_TABLE_H
