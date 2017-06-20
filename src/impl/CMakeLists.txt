foreach (FILE ${TACOPIE_OBJECTS} ${CPP_REDIS_OBJECTS} ${pqxx_OBJECTS})
  set_source_files_properties(
      ${FILE} PROPERTIES
      EXTERNAL_OBJECT TRUE  # Identifies this as an object file
      GENERATED TRUE  # Avoids need for file to exist at configure-time
  )
endforeach (FILE)

add_library(ametsuchi
    $<TARGET_OBJECTS:block_store>
    $<TARGET_OBJECTS:index>
    $<TARGET_OBJECTS:wsv>
    ametsuchi.cpp
    ${pqxx_OBJECTS}
    ${TACOPIE_OBJECTS}
    ${CPP_REDIS_OBJECTS}
    )

add_dependencies(ametsuchi cylix_cpp_redis jtv_libpqxx)

target_include_directories(ametsuchi PUBLIC
    ${pqxx_INCLUDE_DIRS}
    ${CPP_REDIS_INCLUDE_DIRS}
    )

target_link_libraries(ametsuchi
    pq
    )