add_library(utils
  action_parser.h
  action_parser.cc
  block_parser.h
  block_parser.cc
  block_parser_flatbuffers.h
  block_parser_flatbuffers.cc
  hash.h
  hash.cc
  transaction_parser.h
  transcation_parser.cc
  )

target_link_libraries(utils
  flatbuffers
  keccak
  )

target_include_directories(utils INTERFACE .)
