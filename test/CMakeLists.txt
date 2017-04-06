set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/test_bin)

#AddTest(module_keccak module/keccak_test.cc)
#target_link_libraries(module_keccak PRIVATE keccak)

AddTest(module_lmdb module/lmdb_test.cc)
target_link_libraries(module_lmdb PRIVATE LMDB spdlog)

AddTest(currency_test ametsuchi/currency_test.cc)
target_link_libraries(currency_test PRIVATE ${LIBAMETSUCHI_NAME})
#AddTest(module_sqlite module/sqlite_test.cc)
#target_link_libraries(module_sqlite PRIVATE sqlite)
