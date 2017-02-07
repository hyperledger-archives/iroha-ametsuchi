cmake_minimum_required(VERSION 3.0)
project(ametsuchi VERSION 0.1.0 LANGUAGES CXX)

option(AMETSUCHI_COMPILE_SCHEMA "Compile given schema (fbs file) using flatc" OFF)


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


# Standalone executable.
set(AMETSUCHI_NAME ametsuchi_db)
add_executable(${AMETSUCHI_NAME} 
  src/main.cc
)
target_link_libraries(${AMETSUCHI_NAME} 
  ${LIBAMETSUCHI_NAME}
  ${keccak_LINK_DIR}/libkeccak.a
)
add_dependencies(${AMETSUCHI_NAME} spdlog flatbuffers keccak)
set_target_properties(${AMETSUCHI_NAME} PROPERTIES COMPILE_FLAGS -pthread LINK_FLAGS -pthread)
StrictMode(${AMETSUCHI_NAME})




# Connect test executable and CMake test system.
enable_testing()
add_subdirectory(test)

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