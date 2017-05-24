set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/test_bin)

AddTest(wsv_redis_test wsv/wsv_redis_test.cc)
target_link_libraries(wsv_redis_test PRIVATE ${LIBWSV_NAME})
