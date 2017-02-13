cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)
project(ametsuchi VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

option(SCHEMA "Compile given schema (fbs file) using flatc" OFF)
option(BENCHMARKING "Build benchmarks" OFF)
option(TESTING "Build project without tests" OFF)
option(DOCS "Generate API documentation" OFF)


# set default CMAKE_BUILD_TYPE=Debug
if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()


# Detect currently used compiler.
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  set(CMAKE_COMPILER_IS_CLANGCXX 1)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
  set(CMAKE_COMPILER_IS_CLANGCXX 1)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  set(CMAKE_COMPILER_IS_MSVCCXX 1)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
  set(CMAKE_COMPILER_IS_INTELCXX 1)
endif()


include(functions.cmake)
include(dependencies.cmake)


include_directories(
  # Library headers.
  PRIVATE "${PROJECT_SOURCE_DIR}/include"
  # Dependencies.
  PRIVATE "${flatbuffers_SOURCE_DIR}/include"
  PRIVATE "${spdlog_SOURCE_DIR}/include"
  PRIVATE "${keccak_INCLUDE_DIR}"
)


# Library.
set(LIBAMETSUCHI_NAME ametsuchi)
add_library(${LIBAMETSUCHI_NAME}
  ${PROJECT_SOURCE_DIR}/src/ametsuchi/ametsuchi.cc
)
StrictMode(${LIBAMETSUCHI_NAME})


# Documentation.
if(DOCS)
  message(STATUS "Build with docs")
  find_package(Doxygen)
  if(DOXYGEN_NOT_FOUND)
    message(FATAL_ERROR "Doxygen not found")
  endif()
  set(DOC_SOURCES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
  set(DOC_HEADERS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
  configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile.in" Doxyfile
    @ONLY
    NEWLINE_STYLE LF
  )
  add_custom_target(docs ${DOXYGEN_EXECUTABLE})
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
  message(STATUS "Testing is ON")
  enable_testing()
  add_subdirectory(test)
endif(TESTING)


if(BENCHMARKING)
  add_subdirectory(benchmark)
endif(BENCHMARKING)


message(STATUS  "Build type: ${CMAKE_BUILD_TYPE}")
