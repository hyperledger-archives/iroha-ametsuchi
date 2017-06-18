AddTest(bstore_test block_store/block_store_flat_test.cc)
target_link_libraries(bstore_test
  block_store
  )

AddTest(block_index_test index/block_index_test.cc)
target_link_libraries(block_index_test
  index
  )

AddTest(tx_index_test index/tx_index_test.cc)
target_link_libraries(tx_index_test
  index
  )

AddTest(wsv_test wsv/wsv_test.cc)
target_link_libraries(wsv_test
  wsv
  )
