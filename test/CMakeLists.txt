set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/test_bin)

AddTest(ametsuchi_cache_test ametsuchi/cache_test.cc)

AddTest(module_keccak module/keccak_test.cc)
target_link_libraries(module_keccak
  PRIVATE keccak
)
