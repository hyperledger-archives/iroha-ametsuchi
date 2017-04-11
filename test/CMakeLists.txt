set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/test_bin)

AddTest(module_lmdb module/lmdb_test.cc)
target_link_libraries(module_lmdb PRIVATE LMDB spdlog ${LIBAMETSUCHI_NAME})

AddTest(currency_test ametsuchi/currency_test.cc)
target_link_libraries(currency_test PRIVATE ${LIBAMETSUCHI_NAME})

AddTest(ametsuchi_test ametsuchi/ametsuchi.cc)
target_link_libraries(ametsuchi_test PRIVATE ${LIBAMETSUCHI_NAME})

AddTest(merkle_test ametsuchi/merkle_test.cc)
target_link_libraries(merkle_test PRIVATE ${LIBAMETSUCHI_NAME})

AddTest(circular_stack_iter_test ametsuchi/circular_stack_iter_test.cc)
target_link_libraries(circular_stack_iter_test PRIVATE ${LIBAMETSUCHI_NAME})

AddTest(circular_stack_test ametsuchi/circular_stack_test.cc)
target_link_libraries(circular_stack_test PRIVATE ${LIBAMETSUCHI_NAME})
