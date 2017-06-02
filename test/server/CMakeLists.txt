AddTest(wsv_redis_test wsv/wsv_redis_test.cc)
target_link_libraries(wsv_redis_test
  wsv)

AddTest(wsv_postgres_test wsv/wsv_postgres_test.cc)
target_link_libraries(wsv_postgres_test
  wsv)

AddTest(block_index_test block_index/block_index_test.cc)
target_link_libraries(block_index_test
  block_index)

AddTest(tx_index_test tx_index/tx_index_test.cc)
target_link_libraries(tx_index_test
  tx_index)

AddTest(parser_test utils/parser_test.cc)
target_link_libraries(parser_test
  utils)

AddTest(block_parser_flatbuffers_test utils/block_parser_flatbuffers_test.cc)
target_link_libraries(block_parser_flatbuffers_test
  utils)

AddTest(block_parser_protobuf_test utils/block_parser_protobuf_test.cc)
target_link_libraries(block_parser_protobuf_test
  utils)

AddTest(bstore_test block_store/block_store_flat_test.cc)
target_link_libraries(bstore_test
  block_store)

AddTest(sample_client_test service/sample_client_test.cc)
target_link_libraries(sample_client_test
  service
  wsv)
