add_library(ametsuchi
    $<TARGET_OBJECTS:block_store>
    $<TARGET_OBJECTS:index>
    $<TARGET_OBJECTS:wsv>
    ametsuchi.cpp
    ${pqxx_OBJECTS}
    ${TACOPIE_OBJECTS}
    ${CPP_REDIS_OBJECTS}
    )

target_include_directories(ametsuchi PUBLIC
    ${pqxx_INCLUDE_DIRS}
    ${CPP_REDIS_INCLUDE_DIRS}
    )

target_link_libraries(ametsuchi
    pq
    )