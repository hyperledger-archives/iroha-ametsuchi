cmake_minimum_required(VERSION 3.3.0 FATAL_ERROR)
# we need both C and CXX, without C pthread won't be compiled, causing build error
project(ametsuchi VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

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



include(cmake/dependencies.cmake)
include(cmake/functions.cmake)


include_directories(
  # Library headers.
  "${CMAKE_CURRENT_SOURCE_DIR}/include"
)


# Library.
set(LIBAMETSUCHI_NAME ametsuchi)
add_library(${LIBAMETSUCHI_NAME}
  ${PROJECT_SOURCE_DIR}/src/ametsuchi/db.cc
  ${PROJECT_SOURCE_DIR}/src/ametsuchi/io/filevfs.cc
)
target_link_libraries(${LIBAMETSUCHI_NAME}
  flatbuffers
  spdlog
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


# set variable AMETSUCHI_COMPILE_SCHEMA to the path of fbs file
if (EXISTS ${AMETSUCHI_COMPILE_SCHEMA})
  message(STATUS "Compiling ${AMETSUCHI_COMPILE_SCHEMA} schema.")
  message(STATUS "Output will be: ${PROJECT_SOURCE_DIR}/include/ametsuchi/api_generated.h")

  add_custom_command(
    OUTPUT  api_generated.h
    COMMAND ${flatbuffers_BINARY_DIR}/flatc --cpp --scoped-enums ${AMETSUCHI_COMPILE_SCHEMA}
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/include/ametsuchi # includes will be in this directory
    COMMENT "Compiling schema ${AMETSUCHI_COMPILE_SCHEMA}"
  )

  add_custom_target(compileSchema
    DEPENDS api_generated.h
    COMMENT "Generating header file to include/ametsuchi/api_generated.h")

  add_dependencies(${LIBAMETSUCHI_NAME} compileSchema)
endif()


if(TESTING)
  # Connect test executable and CMake test system.
  enable_testing()
  add_subdirectory(test)
endif(TESTING)


if(BENCHMARKING)
  add_subdirectory(benchmark)
endif(BENCHMARKING)



