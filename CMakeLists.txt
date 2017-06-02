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

if (NOT IROHA_SCHEMA_DIR)
  set(IROHA_SCHEMA_DIR ${CMAKE_CURRENT_SOURCE_DIR}/schema)
endif ()
message(STATUS "Schema folder found: ${IROHA_SCHEMA_DIR}")

# set default CMAKE_BUILD_TYPE=Debug
if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif ()
message(STATUS "-DTESTING=${TESTING}")

include_directories(
  "${CMAKE_CURRENT_SOURCE_DIR}/schema"
)
include(cmake/dependencies.cmake)
include(cmake/functions.cmake)

compile_fbs_to_cpp(block.fbs)
compile_proto_to_cpp(block.proto)

add_subdirectory(server)
add_subdirectory(client)

if (TESTING)
  # Connect test executable and CMake test system.
  enable_testing()
  add_subdirectory(test)
endif (TESTING)



