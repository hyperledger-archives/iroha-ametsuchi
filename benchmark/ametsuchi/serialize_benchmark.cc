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
#include <ametsuchi/serializer.h>
#include <benchmark/benchmark.h>
#include <sstream>

static void SerializeMacros(benchmark::State& state) {
  using ametsuchi::ByteArray;
  uint64_t a = 0x2222222222222222;
  uint32_t b = 0x33333333;
  uint16_t c = 0x4444;
  uint8_t d = 0x55;

  ByteArray data(state.range(0), 1u);
  ByteArray a_(state.range(0) + 200u);

  while (state.KeepRunning()) {
    // put
    uint8_t* pw = a_.data();
    PUT_UINT(pw, a, uint64_t);
    PUT_UINT(pw, b, uint32_t);
    PUT_UINT(pw, c, uint16_t);
    PUT_UINT(pw, d, uint8_t);
    PUT_BYTE_ARRAY(pw, data);

    // get
    uint8_t* pr = a_.data();
    GET_UINT(&a, pr, uint64_t);
    GET_UINT(&b, pr, uint32_t);
    GET_UINT(&c, pr, uint16_t);
    GET_UINT(&d, pr, uint8_t);
    GET_BYTE_ARRAY(data, pr);

    assert(a == 0x2222222222222222);
    assert(b == 0x33333333);
    assert(c == 0x4444);
    assert(d == 0x55);
    assert(data[0] == 1);
  }
}

static void SerializeSstream(benchmark::State& state) {
  using ametsuchi::ByteArray;

  uint64_t a = 0x2222222222222222;
  uint32_t b = 0x33333333;
  uint16_t c = 0x4444;
  uint8_t d = 0x55;

  ByteArray data(state.range(0), 1u);
  size_t size = data.size();

  std::stringstream s;

  while (state.KeepRunning()) {
    // put
    s << a << b << c << d;
    s << size;
    for (auto&& b : data) {
      s << b;
    }

    // get
    s >> a >> b >> c >> d;
    s >> size;
    for (size_t i = 0; i < size; i++) {
      s >> data[i];
    }

    assert(a == 0x2222222222222222);
    assert(b == 0x33333333);
    assert(c == 0x4444);
    assert(d == 0x55);
    assert(data[0] == 1);
  }
}

BENCHMARK(SerializeMacros)->Range(1, 1 << 20);
BENCHMARK(SerializeSstream)->Range(1, 1 << 20);
BENCHMARK_MAIN();
