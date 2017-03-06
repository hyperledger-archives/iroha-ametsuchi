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
#include <ametsuchi/globals.h>
#include <chrono>

namespace ametsuchi {

template <typename T>
struct Counter {
  Counter() {}
  void operator=(const Counter&) { ++objects_created; }  // copy, bad
  void operator=(const Counter&&) {}                     // move, ok
  Counter(const Counter&) { ++objects_created; }         // copy, bad
  Counter(const Counter&&) {}                            // move, ok
  static int objects_created;
};

template <typename T>
int Counter<T>::objects_created(0);

class Copyable : public Counter<Copyable> {
 public:
  int x;
  explicit Copyable(int x) : x(x) {}
};

std::list<uint32_t> generateAccessSequence(uint32_t start, uint32_t end,
                                           uint32_t len);

// less copies = better. In our case, we should have 0 copies.
TEST(CacheTest, CountCopies) {
  int max_size = 10;
  Cache<int, Copyable> cache(max_size);

  // pass by address
  for (int i = 0; i < max_size; i++) {
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
  int max_size = 5;
  Cache<int, int> cache(max_size);

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
  int max_size = 10;
  Cache<int, int> cache(max_size);

  for (int i = 0; i < max_size; i++) {
    cache.put(i, i + 1);
    ASSERT_EQ(cache.size(), i + 1u);
  }

  // ensure size decreased
  for (int i = 0; i < max_size; i++) {
    auto size = cache.size();
    auto item = cache.remove(i);
    ASSERT_EQ(cache.size(), size - 1);
    ASSERT_EQ(item, true);
  }
  ASSERT_EQ(cache.size(), 0u);
}

TEST(CacheTest, Clear) {
  int max_size = 10;
  Cache<int, int> cache(max_size);

  for (int i = 0; i < max_size; i++) {
    cache.put(i, i + 1);
  }

  cache.clear();

  ASSERT_EQ(cache.size(), 0u);
}

TEST(CacheTest, PutEvicted) {
  int max_size = 3;
  Cache<int, int> cache(max_size);
  for (int i = 0; i < 3 * max_size; i++) {
    auto evicted = cache.put(i, i + 1);
    if (i < max_size) {
      ASSERT_EQ(evicted, nullptr);
    } else {
      ASSERT_EQ(*evicted, i + 1 - max_size);
    }
  }
}

TEST(CacheTest, Strategy) {
  uint64_t max_size = 2;
  Cache<int, int> cache(max_size);
  cache.put(1, 2);
  cache.put(2, 3);
  // 1-2 2-3
  ASSERT_EQ(*cache.put(3, 4), 2);
  // 3-4 2-3
  cache.setMRU();
  ASSERT_EQ(*cache.put(4, 5), 4);
  // 4-5 2-3
  cache.setLRU();
  ASSERT_EQ(*cache.put(5, 6), 3);
  // 4-5 5-6
}

}  // namespace ametsuchi
