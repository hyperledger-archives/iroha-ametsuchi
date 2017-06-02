add_library(block_store
  block_store.h
  block_store_flat.h
  block_store_flat.cc
  )

target_link_libraries(block_store
  cppfs
  )

target_include_directories(block_store INTERFACE .)
