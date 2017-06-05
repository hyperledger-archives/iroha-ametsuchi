add_library(block_store
  block_store.h
  block_store_flat.h
  flat_iterator.h
  flat_iterator.cc
  block_store_flat.cc
  )

target_link_libraries(block_store
  cppfs
  )

target_include_directories(block_store INTERFACE .)
