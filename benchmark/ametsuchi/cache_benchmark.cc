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

#include <ametsuchi/cache.h>
#include <benchmark/benchmark.h>
#include <stdint.h>
#include <algorithm>

/**
 * Generates a list of `len` random numbers in range [start; end]
 * @param start - min item in sequence
 * @param end - max item in sequence
 * @param len - length of a sequence
 * @return returns std::list of random integers
 */
std::list<uint32_t> generateAccessSequence(uint32_t start, uint32_t end,
                                           uint32_t len) {
  unsigned int seed = 0;
  std::list<uint32_t> ret;
  for (uint32_t i = 0; i < len; i++) {
    uint32_t r = (rand_r(&seed) % (end - start)) + start;
    ret.push_back(r);
  }
  return ret;
}

static void Cache_RandomAccess(benchmark::State& state) {
  // unit of storage
  struct Page {
    char buf[4096];
  };

  auto sequence = generateAccessSequence(0, state.range(0), state.range(1));
  auto begin = sequence.begin();
  auto end = sequence.end();
  ametsuchi::Cache<int, Page> cache(state.range(2));
  uint64_t items = 0;
  while (state.KeepRunning()) {
    // simulate cache
    auto item = cache.get(*begin);
    if (item == nullptr) {
      cache.put(*begin, std::move(Page()));
    }

    state.PauseTiming();
    if (begin == end)
      begin = sequence.begin();
    else
      ++begin;

    items++;
    state.ResumeTiming();
  }

  state.SetBytesProcessed(sizeof(Page) * static_cast<uint64_t>(state.range(1)) *
                          static_cast<uint64_t>(state.iterations()));

  state.SetItemsProcessed(items);
}

/**
 * We generate a sequence of "accesses" to pages (just numbers).
 * It is from 1 << 8 to 1 << 15 (state.range(0))
 * Length of this sequence is from 1 << 15 to 1 << 16 (stage.range(1))
 * {256, 256} - is cache size (items)
 */
BENCHMARK(Cache_RandomAccess)
    ->RangeMultiplier(2)
    ->Ranges({{1 << 8, 1 << 15}, {1 << 15, 1 << 16}, {256, 256}})
    ->Complexity(benchmark::oAuto);

BENCHMARK_MAIN();
