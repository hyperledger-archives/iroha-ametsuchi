cmake_minimum_required(VERSION 3.3.0 FATAL_ERROR)
# we need both C and CXX, without C pthread won't be compiled, causing build error
project(ametsuchi LANGUAGES C CXX)

SET(CMAKE_CXX_FLAGS "-std=c++1y -Wall -fPIC")
SET(CMAKE_CXX_FLAGS_RELEASE "-O3")
SET(CMAKE_CXX_FLAGS_DEBUG "-g -Wextra -Wno-unused-parameter -O0")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules)

option(TESTING "Build tests" ON)

# set default CMAKE_BUILD_TYPE=Debug
if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif ()
message(STATUS "-DTESTING=${TESTING}")

include_directories(
  # Library headers.
  "${CMAKE_CURRENT_SOURCE_DIR}/include"
)
include(cmake/dependencies.cmake)
include(cmake/functions.cmake)

set(AMETSUCHI_INCLUDE
  include/ametsuchi/block_index/block_index.h
  include/ametsuchi/block_index/block_index_redis.h
  include/ametsuchi/block_store/block_store_flat.h
  include/ametsuchi/block_store/block_store.h
  include/ametsuchi/block_store/block_store_nudb.h
  include/ametsuchi/merkle_tree/merkle_tree.h
  include/ametsuchi/tx_index/tx_index.h
  include/ametsuchi/tx_index/tx_index_redis.h
  include/ametsuchi/utils/action_parser.h
  include/ametsuchi/utils/block_parser.h
  include/ametsuchi/utils/transaction_parser.h
  include/ametsuchi/utils/hash.h
  include/ametsuchi/wsv/wsv.h
  include/ametsuchi/wsv/wsv_redis.h
  include/ametsuchi/ametsuchi.h
  )

set(AMETSUCHI_SRC
  src/ametsuchi/utils/block_parser.cc
  src/ametsuchi/utils/action_parser.cc
  src/ametsuchi/utils/transcation_parser.cc
  src/ametsuchi/utils/hash.cc
  src/ametsuchi/ametsuchi.cc
  src/ametsuchi/wsv/wsv_redis.cc
  src/ametsuchi/tx_index/tx_index_redis.cc
  src/ametsuchi/block_store/block_store_nudb.cc
  src/ametsuchi/block_index/block_index_redis.cc
  src/ametsuchi/block_store/block_store_flat.cc
  src/ametsuchi/merkle_tree/merkle_tree.cc)

# Library.
set(LIBAMETSUCHI_NAME ametsuchi)
add_library(${LIBAMETSUCHI_NAME} STATIC
  ${AMETSUCHI_INCLUDE}
  ${AMETSUCHI_SRC}
  )
target_link_libraries(${LIBAMETSUCHI_NAME}
  cpp_redis
  NuDB
  keccak
  )
StrictMode(${LIBAMETSUCHI_NAME})

if (TESTING)
  # Connect test executable and CMake test system.
  enable_testing()
  add_subdirectory(test)
endif (TESTING)



