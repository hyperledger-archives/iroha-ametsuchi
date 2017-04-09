cmake_minimum_required(VERSION 3.3.0 FATAL_ERROR)
# we need both C and CXX, without C pthread won't be compiled, causing build error
project(ametsuchi VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules)

option(SCHEMA "Compile given schema (fbs file) using flatc" OFF)
option(BENCHMARKING "Build benchmarks" OFF)
option(TESTING "Build tests" ON)
option(DOCS "Generate API documentation" OFF)


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
)
include(cmake/dependencies.cmake)
include(cmake/functions.cmake)


set(AMETSUCHI_INCLUDE
  include/ametsuchi/ametsuchi.h
  include/ametsuchi/currency.h
  include/ametsuchi/exception.h
  include/ametsuchi/comparator.h
  include/ametsuchi/merkle_tree/narrow_merkle_tree.h
  include/ametsuchi/merkle_tree/circular_buffer.h

  # needed to compile fbs automatically
  include/ametsuchi/generated/account_generated.h
  include/ametsuchi/generated/asset_generated.h
  include/ametsuchi/generated/commands_generated.h
  include/ametsuchi/generated/main_generated.h
  include/ametsuchi/generated/primitives_generated.h
  include/ametsuchi/generated/transaction_generated.h
  )

set(AMETSUCHI_SRC
  src/ametsuchi/ametsuchi.cc
  src/ametsuchi/currency.cc
  )

# Library.
set(LIBAMETSUCHI_NAME ametsuchi)
add_library(${LIBAMETSUCHI_NAME}
  ${AMETSUCHI_INCLUDE}
  ${AMETSUCHI_SRC}
)
target_link_libraries(${LIBAMETSUCHI_NAME}
  spdlog
  LMDB
  flatbuffers
  #  keccak
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



