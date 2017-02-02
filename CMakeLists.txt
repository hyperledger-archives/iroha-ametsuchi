cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(ametsuchi VERSION 0.1.0 LANGUAGES CXX)

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

# Compile the specified target as a modern, strict C++.
function(StrictMode target)
  # Require pure C++14 standard.
  set_target_properties(${target} PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
  )
  # Enable more warnings and turn them into compile errors.
  if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_CLANGCXX)
    target_compile_options(${target} PRIVATE
      -Wall
      -Wextra
      -Werror
    )
  elseif(CMAKE_COMPILER_IS_MSVCCXX OR CMAKE_COMPILER_IS_INTELCXX)
    target_compile_options(${target} PRIVATE
      /W3
      /WX
    )
  else()
    message(AUTHOR_WARNING
      "Unknown compiler: building target ${target} with default options"
    )
  endif()
endfunction()

# Download all external projects in the specified directory.
include(ExternalProject)
set_directory_properties(PROPERTIES
  EP_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/external"
)
set(EXTERNAL "${CMAKE_CURRENT_SOURCE_DIR}/external/src")

# Project dependencies.
ExternalProject_Add(flatbuffers
  GIT_REPOSITORY "https://github.com/google/flatbuffers.git"
  GIT_TAG "v1.5.0"
  EXCLUDE_FROM_ALL 1
)
ExternalProject_Add(spdlog
  GIT_REPOSITORY "https://github.com/gabime/spdlog.git"
  GIT_TAG "v0.11.0"
  EXCLUDE_FROM_ALL 1
)

# Library.
add_library(libametsuchi
  src/ametsuchi/Greeter.cc
)
target_include_directories(libametsuchi
  # Library headers.
  PUBLIC include
  # Dependencies.
  PRIVATE "${EXTERNAL}/flatbuffers/include"
  PRIVATE "${EXTERNAL}/spdlog/include"
)
StrictMode(libametsuchi)

# Executable.
add_executable(ametsuchi src/main.cc)
target_link_libraries(ametsuchi libametsuchi)
StrictMode(ametsuchi)

# Test dependencies.
ExternalProject_Add(catch
  GIT_REPOSITORY "https://github.com/philsquared/Catch.git"
  GIT_TAG "v.1.7.0"  # The dot after 'v' is intentional.
  EXCLUDE_FROM_ALL 1
)

# Tests.
add_executable(testametsuchi
  test/main.cc
)
target_include_directories(testametsuchi
  PRIVATE "${EXTERNAL}/catch/single_include"
)
target_link_libraries(testametsuchi
  PRIVATE libametsuchi
)
StrictMode(testametsuchi)

# Connect test executable and CMake test system.
enable_testing()
add_test(NAME test COMMAND testametsuchi)

# Documentation.
option(WITH_DOCS "Generate API documentation" ON)
if(WITH_DOCS)
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
endif()
