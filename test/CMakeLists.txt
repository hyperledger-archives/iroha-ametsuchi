set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/test_bin)

AddTest(wsv_redis_test wsv/wsv_redis_test.cc)
target_link_libraries(wsv_redis_test PRIVATE ${LIBAMETSUCHI_NAME})

AddTest(nudb_test tx_store/nudb_test.cc)
target_link_libraries(nudb_test PRIVATE NuDB)

AddTest(tx_redis_test tx_store/tx_redis_test.cc)
target_link_libraries(tx_redis_test PRIVATE ${LIBAMETSUCHI_NAME})