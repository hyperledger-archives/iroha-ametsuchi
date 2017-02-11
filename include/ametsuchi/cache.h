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

#ifndef AMETSUCHI_CACHE_H_
#define AMETSUCHI_CACHE_H_

#include <iterator>
#include <list>
#include <unordered_map>
#include <utility>  // for std::pair

namespace ametsuchi {

template <typename K, typename V>
class Cache {
 public:
  explicit Cache(uint64_t maxSize) : maxCacheSize(maxSize), currentSize(0) {
    _map.reserve(maxSize);
  }

  /**
   * Put item into a cache. If item is already in the cache, its flag "last used
   * time" will be updated.
   * @param key
   * @param value
   */
  void put(K key, V&& value) {
    auto map_iter = _map.find(key);  // O(1)
    if (map_iter != _map.end()) {    // key found
      auto list_iter = map_iter->second;
      if (list_iter != _list.begin()) {  // if item is not MRU
        // then move it to begin, O(1)
        _list.splice(_list.begin(), _list, list_iter, std::next(list_iter));
      }
      list_iter.second = value;


    } else {                                     // key not found
      if (_list.size() == this->maxCacheSize) {  // cache is full
        auto lruPageKey = _list.back().first;    // O(1)
        _map.erase(lruPageKey);                  // O(1)
      }

      // add to the begin of list (now current page is MRU page), O(1)
      _list.push_front(std::make_pair(key, std::forward<V>(value)));
      _map[key] = _list.begin();  // O(1)
    }
  }

  void put(K key, V& value) { this->put(key, std::forward<V>(value)); }

  void put(K key, V* value) { this->put(key, std::forward<V>(*value)); }

  /**
   * Get item from cache. If cache miss - returns nullptr.
   * @param key
   * @return nullptr if item is not in the cache (miss)
   */
  V* get(K key) {
    auto map_iter = _map.find(key);  // O(1)
    if (map_iter != _map.end()) {    // key found
      auto list_iter = map_iter->second;
      if (list_iter != _list.begin()) {  // if item is not MRU
        // then move it to begin, O(1)
        _list.splice(_list.begin(), _list, list_iter, std::next(list_iter));
      }

      return &list_iter->second;

    } else {  // key not found
      return nullptr;
    }
  }

  void print_state(){
    printf("List:\n");
    for(auto it=_list.begin(); it!=_list.end(); it++){
      printf("(%d[%d]) <-> ", it->first, it->second);
    }
    printf("\n\n");
  }

 private:
  using entry_t = typename std::pair<K, V>;
  using iter_t = typename std::list<entry_t>::iterator;

  /**
   * _list.front() = most recently used
   * _list.back()  = least recently used
   */
  std::list<entry_t> _list;
  std::unordered_map<K, iter_t> _map;

  uint64_t maxCacheSize;
  uint64_t currentSize;
};

}  // namespace ametsuchi

#endif  // AMETSUCHI_CACHE_H_
