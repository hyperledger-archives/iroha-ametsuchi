add_library(wsv SHARED
  factory.h
  manager.h
  manager_imp.h
  manager_imp.cc
  wsv.h
  wsv_redis.cc
  wsv_postgres.cc
  )

target_link_libraries(wsv
  cpp_redis
  pqxx
  )

target_include_directories(wsv INTERFACE .)
