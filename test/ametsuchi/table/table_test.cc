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

#include <ametsuchi/table/table.h>
#include <gtest/gtest.h>

namespace ametsuchi {

TEST(Table, RecordSerializer) {
  using table::Record;
  using RecordSerializer = serialize::Serializer<Record>;

  RecordSerializer s;
  Record r(1, 2, 3, {1, 2, 3});

  ByteArray a = s.serialize(&r);

  Record e = s.deserialize(&a);

  ASSERT_EQ(r.tag, e.tag);
  ASSERT_EQ(r.flags, e.flags);
  ASSERT_EQ(r.length, e.length);
  ASSERT_EQ(r.blob.size(), e.blob.size());

  ASSERT_TRUE(r.blob == e.blob);
  ASSERT_EQ(r.blob, e.blob);

  ASSERT_EQ(e.tag, 1);
  ASSERT_EQ(e.flags, 2);
  ASSERT_EQ(e.length, 3);
  ASSERT_EQ(e.blob.size(), 3);
}

}
