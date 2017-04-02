set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/test_bin)

AddTest(cache_test ametsuchi/cache_test.cc)

AddTest(serialize_test ametsuchi/serialize_test.cc)
target_link_libraries(serialize_test PRIVATE spdlog)

AddTest(comparator_test ametsuchi/comparator_test.cc)
target_link_libraries(comparator_test PRIVATE spdlog)

AddTest(module_keccak module/keccak_test.cc)
target_link_libraries(module_keccak PRIVATE keccak)

AddTest(file_test ametsuchi/file/file_test.cc)
target_link_libraries(file_test PRIVATE ${LIBAMETSUCHI_NAME})

AddTest(rw_file_safe_test ametsuchi/file/rw_file_safe_test.cc)
target_link_libraries(rw_file_safe_test PRIVATE ${LIBAMETSUCHI_NAME})

AddTest(circular_buffer_test ametsuchi/circular_buffer_test.cc)
target_link_libraries(circular_buffer_test PRIVATE ${LIBAMETSUCHI_NAME})

AddTest(circular_buffer_iter_test ametsuchi/circular_buffer_iter_test.cc)
target_link_libraries(circular_buffer_iter_test PRIVATE ${LIBAMETSUCHI_NAME})

AddTest(merkle_test ametsuchi/index/merkle_test.cc)
target_link_libraries(merkle_test PRIVATE ${LIBAMETSUCHI_NAME})
