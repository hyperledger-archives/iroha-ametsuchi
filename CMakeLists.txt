cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(ametsuchi VERSION 0.1.0 LANGUAGES CXX)

add_executable(ametsuchi
  src/main.cc
  src/ametsuchi/Greeter.cc
)

target_include_directories(ametsuchi PRIVATE include)

# Require pure C++14 standard.
set_target_properties(ametsuchi PROPERTIES
  CXX_STANDARD 14
  CXX_STANDARD_REQUIRED ON
  CXX_EXTENSIONS OFF
)

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

# Enable more warnings and turn them into compile errors.
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_CLANGCXX)
  target_compile_options(ametsuchi PRIVATE
    -Wall
    -Wextra
    -Werror
  )
elseif(CMAKE_COMPILER_IS_MSVCCXX OR CMAKE_COMPILER_IS_INTELCXX)
  target_compile_options(ametsuchi PRIVATE
    /W3
    /WX
  )
else()
  message(AUTHOR_WARNING
    "Unknown compiler: building project with default options"
  )
endif()
