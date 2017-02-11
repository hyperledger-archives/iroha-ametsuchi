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

#include <gtest/gtest.h>

#include <ametsuchi/cache.h>
#include <string>

namespace ametsuchi {

template <typename T>
struct counter {
  counter() {}
  void operator=(const counter&) { ++objects_created; }
  counter(const counter&) { ++objects_created; }
  counter(const counter&&) {}
  static int objects_created;
};

template <typename T>
int counter<T>::objects_created(0);

class Copyable : public counter<Copyable> {
 public:
  int x;
  explicit Copyable(int x) : x(x) {}

  operator int(){ return x;}

  int get(){ return x;}
};

// less copies = better. In our case, we should have 0 copies.
TEST(CacheTest, CountCopies) {
  int maxSize = 10;
  Cache<int, Copyable> cache(maxSize);

  for (int i = 0; i < maxSize; i++) {
    auto obj = ametsuchi::Copyable(i);
    cache.put(i, obj);
    auto q = cache.get(i);
    if (q) {
      ASSERT_EQ(q->x, obj.x);
    } else {
      ASSERT_EQ(q, nullptr);
    }
  }

  EXPECT_EQ(Copyable::objects_created, 0) << "objects should not be copied";
}

TEST(CacheTest, PutAndGet) {
  int maxSize = 10;
  Cache<int, int> cache(maxSize);

  cache.put(1, 1);
  cache.put(2, 2);
  cache.put(3, 3);
  ASSERT_EQ(*cache.get(1), 1);
  ASSERT_EQ(*cache.get(2), 2);
  ASSERT_EQ(*cache.get(3), 3);
  cache.print_state();

  cache.put(1, 2);
  cache.print_state();
  ASSERT_EQ(*cache.get(1), 2) << "put existing key, but different object";

  ASSERT_EQ(cache.get(4), nullptr) << "non-existent key should return nullptr";
}

}
