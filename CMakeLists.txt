cmake_minimum_required(VERSION 3.3.0 FATAL_ERROR)
# we need both C and CXX, without C pthread won't be compiled, causing build error
project(ametsuchi VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules)

set(SCHEMA_DIR ${CMAKE_CURRENT_BINARY_DIR}/include/ametsuchi)
file(GLOB SCHEMA_FILES ${CMAKE_CURRENT_SOURCE_DIR}/schema/*.fbs)

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
  # Generated FlatBuffers schema.
  "${CMAKE_CURRENT_BINARY_DIR}/include"
)
include(cmake/dependencies.cmake)
include(cmake/functions.cmake)


set(AMETSUCHI_INCLUDE
  include/ametsuchi/file/file.h
  include/ametsuchi/file/rw_file.h
  include/ametsuchi/file/rw_file_safe.h

  include/ametsuchi/ametsuchi.h
  include/ametsuchi/cache.h
  include/ametsuchi/comparator.h
  include/ametsuchi/env.h
  include/ametsuchi/exception.h
  include/ametsuchi/globals.h
  include/ametsuchi/serializer.h
  include/ametsuchi/status.h
  include/ametsuchi/wsv.h
  )

set(AMETSUCHI_SRC
  src/ametsuchi/file/file.cc
  src/ametsuchi/file/rw_file.cc
  src/ametsuchi/file/rw_file_safe.cc

  src/ametsuchi/env.cc
  src/ametsuchi/wsv.cc
  )

# Library.
set(LIBAMETSUCHI_NAME ametsuchi)
add_library(${LIBAMETSUCHI_NAME}
  ${AMETSUCHI_INCLUDE}
  ${AMETSUCHI_SRC}
)
target_link_libraries(${LIBAMETSUCHI_NAME}
  flatbuffers
  spdlog
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


add_custom_target(genfbs
  COMMAND flatc --cpp --scoped-enums -o ${SCHEMA_DIR} ${SCHEMA_FILES}
  COMMENT "Generate headers from FlatBuffers schema")


if(TESTING)
  # Connect test executable and CMake test system.
  enable_testing()
  add_subdirectory(test)
endif(TESTING)


if(BENCHMARKING)
  add_subdirectory(benchmark)
endif(BENCHMARKING)



