set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/test_bin)

AddTest(cache_test ametsuchi/cache_test.cc)

AddTest(table_test ametsuchi/table/table_test.cc)

AddTest(serialize_test ametsuchi/serialize_test.cc)

AddTest(field_table_test ametsuchi/table/field_table_test.cc)
target_link_libraries(field_table_test
        PRIVATE ${LIBAMETSUCHI_NAME}
        )

AddTest(comparator_test ametsuchi/comparator_test.cc)
target_link_libraries(comparator_test
  PRIVATE spdlog
  )

AddTest(module_keccak module/keccak_test.cc)
target_link_libraries(module_keccak
  PRIVATE keccak
)

AddTest(file_test ametsuchi/file/file_test.cc)
target_link_libraries(file_test
  PRIVATE ${LIBAMETSUCHI_NAME}
  )
