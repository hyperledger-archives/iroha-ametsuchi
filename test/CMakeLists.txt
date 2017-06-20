include_directories(.)

addtest(bstore_test block_store/block_store_flat_test.cpp)
target_link_libraries(bstore_test
    ametsuchi
    )

addtest(block_index_test index/block_index_test.cpp)
target_link_libraries(block_index_test
    ametsuchi
    )

addtest(tx_index_test index/tx_index_test.cpp)
target_link_libraries(tx_index_test
    ametsuchi
    )

addtest(wsv_test wsv/wsv_test.cpp)
target_link_libraries(wsv_test
    ametsuchi
    )

addtest(ametsuchi_test ametsuchi_test.cpp)
target_link_libraries(ametsuchi_test
    ametsuchi
    )
