add_library(ametsuchi
  ametsuchi.hpp ametsuchi.cpp)

target_link_libraries(ametsuchi
  block_store
  index
  wsv
  )
