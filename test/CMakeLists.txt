set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/test_bin)

AddTest(wsv_redis_test ametsuchi/wsv/wsv_redis_test.cc)
target_link_libraries(wsv_redis_test PRIVATE ${LIBAMETSUCHI_NAME})

AddTest(postgres_test postgres_test.cc)
target_link_libraries(postgres_test PRIVATE pqxx)

AddTest(tx_redis_test ametsuchi/tx_store/tx_redis_test.cc)
target_link_libraries(tx_redis_test PRIVATE ${LIBAMETSUCHI_NAME})

AddTest(block_index_test ametsuchi/block_index/block_index_test.cc)
target_link_libraries(block_index_test PRIVATE ${LIBAMETSUCHI_NAME})

AddTest(tx_index_test ametsuchi/tx_index/tx_index_test.cc)
target_link_libraries(tx_index_test PRIVATE ${LIBAMETSUCHI_NAME})

AddTest(parser_test ametsuchi/utils/parser_test.cc)
target_link_libraries(parser_test PRIVATE ${LIBAMETSUCHI_NAME})

#AddTest(ametsuchi_test ametsuchi/ametsuchi_test.cc)
#target_link_libraries(ametsuchi_test PRIVATE ${LIBAMETSUCHI_NAME})
