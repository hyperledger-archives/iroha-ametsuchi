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
 * @param start
 * @param end
 * @param len
 * @return
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

static void Cache_PutExisting(benchmark::State& state) {
  ametsuchi::Cache<int, int> cache(64);
  while (state.KeepRunning()) {
    cache.put(1, 1);
  }
}

static void Cache_PutNonExisting(benchmark::State& state) {
  ametsuchi::Cache<int, int> cache(64);
  uint64_t key = 0;
  while (state.KeepRunning()) {
    state.PauseTiming();
    uint32_t r = rand();
    key++;
    state.ResumeTiming();
    cache.put(key, std::move(r));
  }
}

static void Cache_PutRandom(benchmark::State& state) {
  uint32_t maxSize = 64;
  ametsuchi::Cache<uint32_t, uint32_t> cache(maxSize);
  while (state.KeepRunning()) {
    state.PauseTiming();
    uint32_t rk = rand() % maxSize;  // random key
    uint32_t rv = rand() % maxSize;  // random value
    state.ResumeTiming();
    cache.put(rk, std::move(rv));
  }
}

static void Cache_GetExisting(benchmark::State& state) {
  uint32_t maxSize = 64;
  ametsuchi::Cache<uint32_t, uint32_t> cache(maxSize);
  cache.put(1, 100);
  while (state.KeepRunning()) {
    benchmark::DoNotOptimize(cache.get(1));
  }
}

static void Cache_GetNonExisting(benchmark::State& state) {
  uint32_t maxSize = 64;
  ametsuchi::Cache<uint32_t, uint32_t> cache(maxSize);
  cache.put(1, 100);
  while (state.KeepRunning()) {
    benchmark::DoNotOptimize(cache.get(2));
  }
}

static void Cache_GetRandom(benchmark::State& state) {
  uint32_t maxSize = 64;
  ametsuchi::Cache<uint32_t, uint32_t> cache(maxSize);
  auto put = generateAccessSequence(0, maxSize, maxSize);
  std::for_each(put.begin(), put.end(),
                [&cache](uint32_t i) { cache.put(i, std::move(i)); });

  auto get = generateAccessSequence(0, maxSize, maxSize);
  auto it = get.begin();
  while (state.KeepRunning()) {
    cache.get(*it);
    state.PauseTiming();
    if (++it == get.end()) it = get.begin();
    state.ResumeTiming();
  }
}

BENCHMARK(Cache_PutExisting);
BENCHMARK(Cache_PutNonExisting);
BENCHMARK(Cache_PutRandom);
BENCHMARK(Cache_GetExisting);
BENCHMARK(Cache_GetNonExisting);
BENCHMARK(Cache_GetRandom);

BENCHMARK_MAIN();
