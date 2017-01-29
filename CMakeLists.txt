cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(ametsuchi VERSION 0.1.0 LANGUAGES CXX)

add_executable(ametsuchi
  src/main.cc
  src/test.cc
)

target_include_directories(ametsuchi PRIVATE include)

set_target_properties(ametsuchi PROPERTIES
  CXX_STANDARD 14
  CXX_STANDARD_REQUIRED ON
  CXX_EXTENSIONS OFF
)

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  set(CMAKE_COMPILER_IS_CLANGCXX 1)
endif()
if(CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
  set(CMAKE_COMPILER_IS_CLANGCXX 1)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_CLANGCXX)
  target_compile_options(ametsuchi PRIVATE
    -Wall
    -Wextra
    -Werror
  )
else()
  message(AUTHOR_WARNING
    "Unknown compiler: building project with default options"
  )
endif()
