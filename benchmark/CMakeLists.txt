set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/benchmark)

AddBenchmark(cache_benchmark ametsuchi/cache_benchmark.cc)
AddBenchmark(serialize_benchmark ametsuchi/serialize_benchmark.cc)
