add_library(crash_handler
  block_index_mediator.h
  block_index_mediator.cc)

target_link_libraries(crash_handler
  block_index
  block_store
  utils)

target_include_directories(block_store INTERFACE .)