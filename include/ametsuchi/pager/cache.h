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
#include <list>
#include <unordered_map>

namespace ametsuchi {

/**
 * Implementation of LRU (last recently used) in-memory cache.
 * Implements move semantics -- it means, that it should "own" data item:
 *    Example:
 *
 *       Cache<int, Object> cache(1024);
 *
 *       Object obj;
 *
 *       cache.put(1, std::move(obj)); // obj will be nullptr, cache owns memory
 *
 * @tparam K key, everything is accessed through key
 * @tparam V value to be stored in a cache
 */
template <typename K, typename V>
class Cache {
 public:
  explicit Cache(uint64_t max_size);

  /**
   * Put item into a cache. If item is already in the cache, its flag "last used
   * time" will be updated.
   * @param key
   * @param value
   */
  void put(K key, V&& value);

  /**
   * Get item from cache. If cache miss - returns nullptr.
   * @param key
   * @return nullptr if item is not in the cache (miss)
   */
  V* get(K key);

  /**
   * Removes item from a cache.
   * @param key to be removed
   * @return true if key is found and removed, false otherwise
   */
  bool remove(K key);

  /**
   * Clears the cache.
   */
  void clear();

  /**
   * Returns current size of cache.
   * @return
   */
  uint64_t size();

 private:
  using Entry = typename std::pair<K, V>;
  using Iter = typename std::list<Entry>::iterator;

  /**
   * contains std::pair<key, value>
   * lru_list.front() = most recently used
   * lru_list.back()  = least recently used
   */
  std::list<Entry> lru_list;

  /**
   * maps keys to iterators in lru_list
   */
  std::unordered_map<K, Iter> iters_map;

  uint64_t max_cache_size;
  uint64_t current_size;
};

#include "cache.inc"

}  // namespace ametsuchi
