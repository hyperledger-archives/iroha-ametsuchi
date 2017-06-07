add_library(service
  storage_service.h
  storage_service.cc
  )

target_link_libraries(service
  block_store
  wsv
  grpc++
  protobuf
  )

target_include_directories(service INTERFACE .)
