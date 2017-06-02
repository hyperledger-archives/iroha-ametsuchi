add_library(wsv
  wsv.h
  wsv_redis.h
  wsv_redis.cc
  wsv_postgres.h
  wsv_postgres.cc
  )

target_link_libraries(wsv
  cpp_redis
  pqxx
  )

target_include_directories(wsv INTERFACE .)
