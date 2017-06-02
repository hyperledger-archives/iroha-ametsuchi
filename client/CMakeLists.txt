project(libametsuchiclient)

add_library(ametsuchiclient
  include/ametsuchi/ametsuchi.h
  include/ametsuchi/client.h
  src/ametsuchi/client.cc
  )

target_link_libraries(ametsuchiclient PUBLIC
  protobuf
  grpc++
  )

target_include_directories(ametsuchiclient PUBLIC
  include
  )
