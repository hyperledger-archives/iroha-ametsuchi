add_library(tx_index
  tx_index.h
  tx_index_redis.h
  tx_index_redis.cc
  )

target_link_libraries(tx_index
  cpp_redis
  optional
  )

target_include_directories(tx_index INTERFACE .)
