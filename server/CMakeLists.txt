project(ametsuchi-server)

add_subdirectory(block_index)
add_subdirectory(block_store)
add_subdirectory(tx_index)
add_subdirectory(utils)
add_subdirectory(wsv)

add_subdirectory(service)

add_executable(ametsuchi-server
  main.cc
  )

target_link_libraries(ametsuchi-server PRIVATE
  service
  )
