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

#include <ametsuchi/table/field_table.h>

namespace ametsuchi {
namespace table {


    FieldTable::ForwardIterator::ForwardIterator(FieldTable& ft): ft_(ft) {
        offset_ = 0;
        value_ = ft_.get(offset_);
    }

    FieldTable::ForwardIterator::ForwardIterator(FieldTable& ft, offset_t offset) : ft_(ft) {
        offset_ = offset;
        value_ = ft.get(offset_);
    }

    FieldTable::ForwardIterator::ForwardIterator(const FieldTable::ForwardIterator &it): offset_(it.offset_), value_(it.value_), ft_(it.ft_){
    }

    FieldTable::ForwardIterator &FieldTable::ForwardIterator::operator++() {
        return ForwardIterator(offset_+(4+value_.size()));
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

    FieldTable::ForwardIterator FieldTable::begin() {
        return ForwardIterator(*this);
    }

}
}