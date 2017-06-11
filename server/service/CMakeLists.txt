add_library(service
  storage_service.h
  storage_service.cc
  )

target_link_libraries(service
  block_store
  block_index
  tx_index
  wsv
  grpc++
  protobuf
  utils
  )

target_include_directories(service INTERFACE .)
