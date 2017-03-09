set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/benchmark)

AddBenchmark(cache_benchmark ametsuchi/cache_benchmark.cc)
AddBenchmark(serialize_benchmark ametsuchi/serialize_benchmark.cc)
AddBenchmark(file_benchmark ametsuchi/file_benchmark.cc)

target_link_libraries(file_benchmark
  PRIVATE ${LIBAMETSUCHI_NAME}
  )
