set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/test_bin)

AddTest(ametsuchi_cache_test ametsuchi/pager/cache_test.cc)
