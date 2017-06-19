add_library(block_store
  backend/flat_file.cpp
  )

target_link_libraries(block_store
  stdc++fs # std::experimental::filesystem
  )
