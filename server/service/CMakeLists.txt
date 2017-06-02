add_library(service
  query_service.h
  query_service.cc
  )

target_link_libraries(service
  wsv
  grpc++
  )

target_include_directories(service INTERFACE .)
