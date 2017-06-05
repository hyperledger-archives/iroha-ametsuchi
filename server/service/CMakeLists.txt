add_library(service
  command_service.h
  command_service.cc
  query_service.h
  query_service.cc
  )

target_link_libraries(service
  block_store
  wsv
  grpc++
  protobuf
  )

target_include_directories(service INTERFACE .)
