set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/test_bin)

AddTest(ametsuchi_cache_test ametsuchi/cache_test.cc)
AddTest(ametsuchi_database_test ametsuchi/db_test.cc)
