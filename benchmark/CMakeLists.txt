set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/benchmark)

add_executable(cache_benchmark ametsuchi/cache_benchmark.cc)
add_dependencies(cache_benchmark gbenchmark)
target_include_directories(cache_benchmark PRIVATE ${gbenchmark_INCLUDE_DIR})
target_link_libraries(cache_benchmark ${gbenchmark_LINK_FILES})
StrictMode(cache_benchmark)
