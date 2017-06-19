add_library(ametsuchi
    ../block_store/backend/flat_file.cpp
    ../index/backend/redis.cpp
    ../wsv/backend/postgresql.cpp
    ametsuchi.cpp
    )

target_link_libraries(ametsuchi PUBLIC
    pqxx
    cpp_redis
    stdc++fs
    )
