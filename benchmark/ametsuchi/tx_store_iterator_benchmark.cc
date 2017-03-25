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

#include <ametsuchi/globals.h>
#include <ametsuchi/tx_store/array.h>
#include <ametsuchi/tx_store/iterator.h>
#include <benchmark/benchmark.h>

static void AppendFunction(benchmark::State& state) {
  using ametsuchi::ByteArray;
  using ametsuchi::tx_store::Array;

  const std::string array_path = "/tmp/array";
  ByteArray test_set;
  for (int j = 0; j < 500; ++j) {
    test_set.push_back(1);
  }
  const int N = state.range(0);

  while (state.KeepRunning()) {
    Array array_(array_path);

    for (int i = 0; i < N; ++i) {
      array_.append(test_set);
    }
    array_.commit();
  }
  remove(array_path.c_str());
  remove((array_path+"_index").c_str());
}

static void GetFunction(benchmark::State& state) {
  using ametsuchi::ByteArray;
  using ametsuchi::tx_store::Array;

  const std::string array_path = "/tmp/array";

  const int N = state.range(0);
  ByteArray test_set;
  for (int j = 0; j < 500; ++j) {
    test_set.push_back(1);
  }

  Array array_(array_path);
  for (int i = 0; i < N; ++i) {
    array_.append(test_set);
  }
  array_.commit();

  while(state.KeepRunning()){
    for (int i = 0; i < N; ++i){
      assert(array_.get(i) == test_set);
    }
  }
  remove(array_path.c_str());
  remove((array_path+"_index").c_str());
}

BENCHMARK(AppendFunction)->Range(1, 1 << 20);
BENCHMARK(GetFunction)->Range(1, 1 << 20);
BENCHMARK_MAIN()