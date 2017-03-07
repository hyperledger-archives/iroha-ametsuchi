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

static void SerializeFunction(benchmark::State& state) {
  using ametsuchi::ByteArray;
  using ametsuchi::serialize::put;
  using ametsuchi::serialize::get;

  uint64_t a = 0x2222222222222222;
  uint32_t b = 0x33333333;
  uint16_t c = 0x4444;
  uint8_t d = 0x55;

  ByteArray data(state.range(0), 1u);
  ByteArray a_(state.range(0) + 200u);

  while (state.KeepRunning()) {
    // put
    ametsuchi::byte_t* pw = a_.data();
    put(pw, a);
    put(pw, b);
    put(pw, c);
    put(pw, d);
    put(pw, data);

    // get
    const ametsuchi::byte_t* pr = a_.data();
    get(&a, pr);
    get(&b, pr);
    get(&c, pr);
    get(&d, pr);
    get(&data, pr);

    assert(a == 0x2222222222222222);
    assert(b == 0x33333333);
    assert(c == 0x4444);
    assert(d == 0x55);
    assert(data[0] == 1);
  }
}

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
    s.write(reinterpret_cast<const char*>(&a), sizeof(a));
    s.write(reinterpret_cast<const char*>(&b), sizeof(b));
    s.write(reinterpret_cast<const char*>(&c), sizeof(c));
    s.write(reinterpret_cast<const char*>(&d), sizeof(d));
    s.write(reinterpret_cast<const char*>(&size), sizeof(size));
    for (auto&& b : data) {
      s.write(reinterpret_cast<const char*>(&b), sizeof(b));
    }

    // get
    s.read(reinterpret_cast<char*>(&a), sizeof(a));
    s.read(reinterpret_cast<char*>(&b), sizeof(b));
    s.read(reinterpret_cast<char*>(&c), sizeof(c));
    s.read(reinterpret_cast<char*>(&d), sizeof(d));
    s.read(reinterpret_cast<char*>(&size), sizeof(size));
    for (size_t i = 0; i < size; i++) {
      s.read(reinterpret_cast<char*>(data.data() + i),
             sizeof(ametsuchi::byte_t));
    }

    assert(a == 0x2222222222222222);
    assert(b == 0x33333333);
    assert(c == 0x4444);
    assert(d == 0x55);
    assert(data[0] == 1);
  }
}

BENCHMARK(SerializeFunction)->Range(1, 1 << 20);
BENCHMARK(SerializeMacros)->Range(1, 1 << 20);
BENCHMARK(SerializeSstream)->Range(1, 1 << 20);
BENCHMARK_MAIN()
