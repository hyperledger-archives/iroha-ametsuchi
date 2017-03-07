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

        FieldTable::FieldTable(file::SequentialFile &seqFile, file::AppendableFile &appFile): seqFile_(seqFile), appFile_(appFile), fileLength(0){}

        offset_t FieldTable::put(const ByteArray &value) {
            appFile_.open();
            ByteArray memory(1024); // assume size of value would fit into 4 bytes

            uint8_t* ptr = memory.data();

            PUT_BYTE_ARRAY(ptr, value);

            memory.resize(8+value.size());
            appFile_.append(memory);
            appFile_.close();

            offset_t offsetToThisValue = fileLength;
            fileLength += memory.size();
            return offsetToThisValue;
        }

        ByteArray FieldTable::get(const offset_t offset) {
            seqFile_.open();

            ByteArray bytes = seqFile_.read(1024, offset);

            const uint8_t* p = bytes.data();
            ByteArray value = {0};
            GET_BYTE_ARRAY(value, p);
            seqFile_.close();
            return value;
        }

        FieldTable::ForwardIterator FieldTable::begin() {
            return ForwardIterator(*this);
        }

        FieldTable::ForwardIterator FieldTable::end() {
            auto fi = ForwardIterator(*this, fileLength);
            return fi;
        }

        FieldTable::ForwardIterator::ForwardIterator(FieldTable& ft): ft_(ft) {
            offset_ = 0;
            value_ = ft_.get(offset_);
        }

        FieldTable::ForwardIterator::ForwardIterator(FieldTable &ft, offset_t offset): ft_(ft), offset_(offset) {
            value_ = ft_.get(offset);
        }

//        FieldTable::ForwardIterator::~ForwardIterator() {}

        FieldTable::ForwardIterator::ForwardIterator(const FieldTable::ForwardIterator &it):
                ft_(it.ft_), offset_(it.offset_), value_(it.value_){
        }

        FieldTable::ForwardIterator &FieldTable::ForwardIterator::operator++() {
            offset_ += value_.size() + 8;
            value_ = ft_.get(offset_);
            return *this;
        }

        FieldTable::ForwardIterator FieldTable::ForwardIterator::operator++(int) {
            ForwardIterator iterator(*this);
            offset_ += value_.size() + 8;
            value_ = ft_.get(offset_);
            return iterator;
        }

        ByteArray &FieldTable::ForwardIterator::operator*() {
            return value_;
        }

        bool FieldTable::ForwardIterator::operator==(const FieldTable::ForwardIterator &it) {
            return offset_ == it.offset_;
        }

        bool FieldTable::ForwardIterator::operator<(const FieldTable::ForwardIterator &it) {
            return offset_ < it.offset_;
        }
    }
}

#endif //AMETSUCHI_FIELD_TABLE_H
