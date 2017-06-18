add_library(index
  index.hpp
  backend/redis.hpp backend/redis.cpp
  )

target_link_libraries(index
  cpp_redis
  )
