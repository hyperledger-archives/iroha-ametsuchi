set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/test_bin)

AddTest(ametsuchi_cache_test ametsuchi/pager/cache_test.cc)
AddTest(ametsuchi_db_test ametsuchi/db_test.cc)
AddTest(ametsuchi_filevfs_test ametsuchi/io/filevfs_test.cc)
AddTest(module_keccak module/keccak_test.cc)
