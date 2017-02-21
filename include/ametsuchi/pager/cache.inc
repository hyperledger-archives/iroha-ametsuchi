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

template <typename K, typename V>
Cache<K, V>::Cache(uint64_t max_size) : max_cache_size(max_size), current_size(0) {
  iters_map.reserve(max_size);
}

template <typename K, typename V>
void Cache<K, V>::put(K key, V&& value) {
  auto map_iter = iters_map.find(key);  // O(1)
  if (map_iter != iters_map.end()) {    // key found
    auto list_iter = map_iter->second;
    if (list_iter != lru_list.begin()) {  // if item is not MRU
      // then move it to begin, O(1)
      lru_list.splice(lru_list.begin(), lru_list, list_iter, std::next(list_iter));
    }
    list_iter->second = value;
  } else {                                    // key not found
    if (current_size == this->max_cache_size) {  // cache is full
      auto lruPageKey = lru_list.back().first;   // O(1)
      iters_map.erase(lruPageKey);                 // O(1)
      lru_list.pop_back();
      current_size--;
    }

    // add to the begin of list (now current page is MRU page), O(1)
    lru_list.push_front(std::make_pair(key, std::forward<V>(value)));
    iters_map[key] = lru_list.begin();  // O(1)
    current_size++;
  }
}

template <typename K, typename V>
V* Cache<K, V>::get(K key) {
  auto map_iter = iters_map.find(key);  // O(1)
  if (map_iter != iters_map.end()) {    // key found
    auto list_iter = map_iter->second;
    if (list_iter != lru_list.begin()) {  // if item is not MRU
      // then move it to begin, O(1)
      lru_list.splice(lru_list.begin(), lru_list, list_iter, std::next(list_iter));
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
uint64_t Cache<K, V>::size() { return current_size; }
