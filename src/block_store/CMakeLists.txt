set(block_store_SOURCES
    backend/flat_file.cpp
    )

add_library(block_store
  ${block_store_SOURCES}
  )

target_link_libraries(block_store
  stdc++fs # std::experimental::filesystem
  )
