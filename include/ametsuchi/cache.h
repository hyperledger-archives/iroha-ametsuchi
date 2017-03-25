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
#include <utility>  // for std::pair

namespace ametsuchi {
/**
 * Implementation of LRU (least recently used) / MRU (most recently used) in-memory cache.
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
  explicit Cache(uint64_t max_size, bool isMRU = false);

  /**
   * Put item into a cache. If item is already in the cache, its flag "last used
   * time" will be updated.
   * @param key
   * @param value
   * @return nullptr if nothing was evicted from cache, pointer to evicted
   * object otherwise
   */
  std::unique_ptr<V> put(K key, V&& value);

  /**
   * Get item from cache. If cache miss - returns nullptr.
   * @param key
   * @return nullptr if item is not in the cache (miss)
   */
  const V* get(K key);

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

  /**
   * Sets LRU eviction strategy
   */
  void setLRU();

  /**
   * Sets MRU eviction strategy
   */
  void setMRU();

  /**
   * Set cache max size
   * @param new_size
   */
  void setMaxCacheSize(uint64_t new_size);

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
  bool isMRU;

};

/// IMPLEMENTATION

template <typename K, typename V>
Cache<K, V>::Cache(uint64_t max_size, bool isMRU)
    : max_cache_size(max_size), current_size(0), isMRU(isMRU) {
  iters_map.reserve(max_size);
}

template <typename K, typename V>
std::unique_ptr<V> Cache<K, V>::put(K key, V&& value) {
  auto map_iter = iters_map.find(key);  // O(1)
  if (map_iter != iters_map.end()) {    // key found
    auto list_iter = map_iter->second;
    if (list_iter != lru_list.begin()) {  // if item is not MRU
      // then move it to begin, O(1)
      lru_list.splice(lru_list.begin(), lru_list, list_iter,
                      std::next(list_iter));
    }

    list_iter->second = value;

    return nullptr;
  } else {  // key not found
    std::unique_ptr<V> evicted;
    if (current_size == this->max_cache_size) {  // cache is full
      auto& element = isMRU ? lru_list.front() : lru_list.back();  // O(1)
      evicted.reset(new V{element.second});      // move evicted value to the new place
      iters_map.erase(element.first);  // O(1)
      if (isMRU) {
        lru_list.pop_front();
      } else {
        lru_list.pop_back();
      }
      current_size--;
    }

    // add to the begin of list (now current page is MRU page), O(1)
    lru_list.push_front(std::make_pair(key, std::forward<V>(value)));
    iters_map[key] = lru_list.begin();  // O(1)
    current_size++;

    return evicted;
  }
}

template <typename K, typename V>
const V* Cache<K, V>::get(K key) {
  auto map_iter = iters_map.find(key);  // O(1)
  if (map_iter != iters_map.end()) {    // key found
    auto list_iter = map_iter->second;
    if (list_iter != lru_list.begin()) {  // if item is not MRU
      // then move it to begin, O(1)
      lru_list.splice(lru_list.begin(), lru_list, list_iter,
                      std::next(list_iter));
    }
    return &list_iter->second;

  } else {  // key not found
    return nullptr;
  }
}

template <typename K, typename V>
bool Cache<K, V>::remove(K key) {
  auto map_iter = iters_map.find(key);  // O(1)
  if (map_iter != iters_map.end()) {    // key found
    auto list_iter = map_iter->second;
    lru_list.erase(list_iter);
    iters_map.erase(key);
    current_size--;
    return true;
  } else {  // key not found
    return false;
  }
}

template <typename K, typename V>
void Cache<K, V>::clear() {
  lru_list.clear();
  iters_map.clear();
  current_size = 0;
}

template <typename K, typename V>
uint64_t Cache<K, V>::size() {
  return current_size;
}

template <typename K, typename V>
void Cache<K, V>::setLRU() {
  isMRU = false;
}

template <typename K, typename V>
void Cache<K, V>::setMRU() {
  isMRU = true;
}

template <typename K, typename V>
void Cache<K, V>::setMaxCacheSize(uint64_t new_size) {
  max_cache_size = new_size;
  iters_map.reserve(new_size);
}



}  // namespace ametsuchi
