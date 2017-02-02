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

# Library.
set(LIBAMETSUCHI_NAME libametsuchi)
add_library(${LIBAMETSUCHI_NAME}
  src/ametsuchi/Greeter.cc
)
StrictMode(libametsuchi)



# Project dependencies.
# flatbuffers
ExternalProject_Add(flatbuffers
  GIT_REPOSITORY "https://github.com/google/flatbuffers.git"
  GIT_TAG "v1.5.0"
  INSTALL_COMMAND   ""
  TEST_COMMAND      ""
)
ExternalProject_Get_Property(flatbuffers source_dir)
set(flatbuffers_SOURCE_DIR "${source_dir}")


# speedlog
ExternalProject_Add(spdlog
  GIT_REPOSITORY "https://github.com/gabime/spdlog.git"
  GIT_TAG "v0.11.0"
  #CONFIGURE_COMMAND ""
  #BUILD_COMMAND     ""
  INSTALL_COMMAND   ""
  TEST_COMMAND      ""
)
ExternalProject_Get_Property(spdlog source_dir)
set(spdlog_SOURCE_DIR "${source_dir}")



include_directories(
  # Library headers.
  PRIVATE "${PROJECT_SOURCE_DIR}/include"
  # Dependencies.
  PRIVATE "${flatbuffers_SOURCE_DIR}/include"
  PRIVATE "${spdlog_SOURCE_DIR}/include"
)



# Executable.
set(AMETSUCHI_NAME ametsuchi)
add_executable(${AMETSUCHI_NAME} 
  src/main.cc
)

target_link_libraries(${AMETSUCHI_NAME} 
  libametsuchi
)
add_dependencies(${AMETSUCHI_NAME} spdlog flatbuffers)
set_target_properties(${AMETSUCHI_NAME} PROPERTIES COMPILE_FLAGS -pthread LINK_FLAGS -pthread)
StrictMode(ametsuchi)



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
