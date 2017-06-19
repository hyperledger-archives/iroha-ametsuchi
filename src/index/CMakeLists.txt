add_library(index
    backend/redis.cpp
    )

target_link_libraries(index
    cpp_redis
    )
