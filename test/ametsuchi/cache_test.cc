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
#include <stdint.h>
#include <stdlib.h>
#include <chrono>
#include <string>
#include <vector>

namespace ametsuchi {

template <typename T>
struct counter {
  counter() {}
  void operator=(const counter&) { ++objects_created; }  // copy, bad
  void operator=(const counter&&) {}                     // move, ok
  counter(const counter&) { ++objects_created; }         // copy, bad
  counter(const counter&&) {}                            // move, ok
  static int objects_created;
};

template <typename T>
int counter<T>::objects_created(0);

class Copyable : public counter<Copyable> {
 public:
  int x;
  explicit Copyable(int x) : x(x) {}
};

std::list<uint32_t> generateAccessSequence(uint32_t start, uint32_t end,
                                           uint32_t len);

// less copies = better. In our case, we should have 0 copies.
TEST(CacheTest, CountCopies) {
  int maxSize = 10;
  Cache<int, Copyable> cache(maxSize);

  // pass by address
  for (int i = 0; i < maxSize; i++) {
    Copyable obj(i);
    cache.put(i, std::move(obj));
    auto q = cache.get(i);
    ASSERT_NE(q, nullptr);
    ASSERT_EQ(q->x, i);
  }

  // we moved temp items to cache, check if we still can access them after their
  auto q = cache.get(1);
  ASSERT_NE(q, nullptr);
  ASSERT_EQ(q->x, 1);

  EXPECT_EQ(Copyable::objects_created, 0) << "objects should not be copied";
}

TEST(CacheTest, PutAndGet) {
  int maxSize = 5;
  Cache<int, int> cache(maxSize);

  cache.put(1, 1);
  cache.put(2, 2);
  cache.put(3, 3);
  EXPECT_EQ(*cache.get(1), 1);
  EXPECT_EQ(*cache.get(2), 2);
  EXPECT_EQ(*cache.get(3), 3);
  EXPECT_EQ(*cache.get(1), 1);

  cache.put(1, 2);
  EXPECT_EQ(*cache.get(1), 2) << "put existing key, but different value";

  EXPECT_EQ(cache.get(4), nullptr) << "non-existent key should return nullptr";

  // add more than MAX items
  cache.put(1, 1);  // will be removed
  cache.put(2, 2);  // will be removed
  cache.put(3, 3);  // will be removed
  cache.put(4, 4);  // 5
  cache.put(5, 5);  // 4
  cache.put(6, 6);  // 3
  cache.put(7, 7);  // 2
  cache.put(8, 8);  // 1

  EXPECT_EQ(cache.get(1), nullptr);
  EXPECT_EQ(cache.get(2), nullptr);
  EXPECT_EQ(cache.get(3), nullptr);
  EXPECT_EQ(*cache.get(4), 4);
  EXPECT_EQ(*cache.get(5), 5);
  EXPECT_EQ(*cache.get(6), 6);
  EXPECT_EQ(*cache.get(7), 7);
  EXPECT_EQ(*cache.get(8), 8);
}

TEST(CacheTest, RemoveAndSize) {
  int maxSize = 10;
  Cache<int, int> cache(maxSize);

  for (int i = 0; i < maxSize; i++) {
    cache.put(i, i + 1);
    ASSERT_EQ(cache.size(), i + 1u);
  }

  // ensure size decreased
  for (int i = 0; i < maxSize; i++) {
    auto size = cache.size();
    auto item = cache.remove(i);
    ASSERT_EQ(cache.size(), size - 1);
    ASSERT_EQ(item, true);
  }
  ASSERT_EQ(cache.size(), 0u);
}

TEST(CacheTest, Clear){
  int maxSize = 10;
  Cache<int, int> cache(maxSize);

  for (int i = 0; i < maxSize; i++) {
    cache.put(i, i + 1);
  }

  cache.clear();

  ASSERT_EQ(cache.size(), 0u);
}

}  // namespace ametsuchi
