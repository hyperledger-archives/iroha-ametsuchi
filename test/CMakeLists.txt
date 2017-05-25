set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/test_bin)

AddTest(wsv_redis_test ametsuchi/wsv/wsv_redis_test.cc)
target_link_libraries(wsv_redis_test PRIVATE ${LIBAMETSUCHI_NAME})

AddTest(nudb_test ametsuchi/tx_store/nudb_test.cc)
target_link_libraries(nudb_test PRIVATE NuDB)

AddTest(tx_redis_test ametsuchi/tx_store/tx_redis_test.cc)
target_link_libraries(tx_redis_test PRIVATE ${LIBAMETSUCHI_NAME})

AddTest(block_store_nudb_test ametsuchi/block_store/block_store_nudb_test.cc)
target_link_libraries(block_store_nudb_test PRIVATE ${LIBAMETSUCHI_NAME})
