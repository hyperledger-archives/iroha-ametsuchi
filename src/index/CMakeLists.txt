set(index_SOURCES
    backend/redis.cpp
    )

add_library(index
  ${index_SOURCES}
  )

target_link_libraries(index
  cpp_redis
  )
