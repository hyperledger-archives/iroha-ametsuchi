add_library(block_store
  block_store.hpp
  backend/flat_file.hpp backend/flat_file.cpp
  )

target_link_libraries(block_store
  stdc++fs # std::experimental::filesystem
  )
