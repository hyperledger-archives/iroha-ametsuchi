add_library(utils
#  action_parser.h
#  action_parser.cc
#  block_parser.h
#  block_parser.cc
#  block_parser_protobuf.h
#  block_parser_protobuf.cc
  hash.h
  hash.cc
#  transaction_parser.h
#  transcation_parser.cc
  )

target_link_libraries(utils
  protobuf
  keccak
  )

target_include_directories(utils INTERFACE .)
