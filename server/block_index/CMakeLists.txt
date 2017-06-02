add_library(block_index
  block_index.h
  block_index_redis.h
  block_index_redis.cc
  )

target_link_libraries(block_index
  cpp_redis
  )

target_include_directories(block_index INTERFACE .)
