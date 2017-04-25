cmake_minimum_required(VERSION 3.3.0 FATAL_ERROR)
# we need both C and CXX, without C pthread won't be compiled, causing build error
project(ametsuchi VERSION 0.1.0 LANGUAGES C CXX)

SET(CMAKE_CXX_FLAGS         "-std=c++1y -Wall -fPIC")
SET(CMAKE_CXX_FLAGS_RELEASE "-O3")
SET(CMAKE_CXX_FLAGS_DEBUG   "-g -Wextra -Wno-unused-parameter -O0")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules)


option(BENCHMARKING "Build benchmarks" OFF)
option(TESTING "Build tests" ON)
option(DOCS "Generate API documentation" OFF)

if(NOT IROHA_SCHEMA_DIR)
  set(IROHA_SCHEMA_DIR ${PROJECT_SOURCE_DIR}/schema)
endif()
message(STATUS "Schema folder found: ${IROHA_SCHEMA_DIR}")

# set default CMAKE_BUILD_TYPE=Debug
if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()
message(STATUS "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}")
message(STATUS "-DTESTING=${TESTING}")
message(STATUS "-DBENCHMARKING=${BENCHMARKING}")
message(STATUS "-DDOCS=${DOCS}")


include_directories(
  # Library headers.
  "${CMAKE_CURRENT_SOURCE_DIR}/include"
  "${IROHA_SCHEMA_DIR}"
)
include(cmake/dependencies.cmake)
include(cmake/functions.cmake)


set(AMETSUCHI_INCLUDE
  include/ametsuchi/ametsuchi.h
  include/ametsuchi/tx_store.h
  include/ametsuchi/wsv.h
  include/ametsuchi/common.h
  include/ametsuchi/currency.h
  include/ametsuchi/exception.h
  include/ametsuchi/comparator.h
  include/ametsuchi/merkle_tree/narrow_merkle_tree.h
  include/ametsuchi/merkle_tree/circular_stack.h
  include/ametsuchi/merkle_tree/merkle_tree.h

  # needed to compile fbs automatically
  schema/account_generated.h
  schema/asset_generated.h
  schema/commands_generated.h
  schema/main_generated.h
  schema/primitives_generated.h
  schema/transaction_generated.h
  )

set(AMETSUCHI_SRC
  src/ametsuchi/ametsuchi.cc
  src/ametsuchi/tx_store.cc
  src/ametsuchi/wsv.cc
  src/ametsuchi/currency.cc
  src/ametsuchi/common.cc
  src/ametsuchi/merkle_tree/merkle_tree.cc
  )

# Library.
set(LIBAMETSUCHI_NAME ametsuchi)
add_library(${LIBAMETSUCHI_NAME} SHARED
  ${AMETSUCHI_INCLUDE}
  ${AMETSUCHI_SRC}
)
target_link_libraries(${LIBAMETSUCHI_NAME}
  spdlog
  LMDB
  flatbuffers
  keccak
  )
StrictMode(${LIBAMETSUCHI_NAME})


# Documentation.
if(DOCS)
  find_package(Doxygen)
  if(DOXYGEN_NOT_FOUND)
    message(FATAL_ERROR "Doxygen not found")
  endif()
  set(DOC_SOURCES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
  set(DOC_HEADERS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
  add_custom_target(
    doc ALL
    COMMAND ${DOXYGEN_EXECUTABLE}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Generating API documentation with Doxygen"
    VERBATIM
  )
endif(DOCS)


if(TESTING)
  # Connect test executable and CMake test system.
  enable_testing()
  add_subdirectory(test)
endif(TESTING)


if(BENCHMARKING)
  add_subdirectory(benchmark)
endif(BENCHMARKING)



