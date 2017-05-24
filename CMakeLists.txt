cmake_minimum_required(VERSION 3.3.0 FATAL_ERROR)
# we need both C and CXX, without C pthread won't be compiled, causing build error
project(ametsuchi LANGUAGES C CXX)

SET(CMAKE_CXX_FLAGS         "-std=c++1y -Wall -fPIC")
SET(CMAKE_CXX_FLAGS_RELEASE "-O3")
SET(CMAKE_CXX_FLAGS_DEBUG   "-g -Wextra -Wno-unused-parameter -O0")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules)


option(TESTING "Build tests" ON)

# set default CMAKE_BUILD_TYPE=Debug
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()
message(STATUS "-DTESTING=${TESTING}")

include_directories(
        # Library headers.
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)
include(cmake/dependencies.cmake)
include(cmake/functions.cmake)


set(AMETSUCHI_INCLUDE
        include/wsv/wsv.h
        include/wsv/wsv_redis.h
        include/tx_store/tx_redis.h
        )

set(AMETSUCHI_SRC
        src/wsv/wsv_redis.cc
        src/tx_store/tx_redis.cc)

# Library.
set(LIBAMETSUCHI_NAME ametsuchi)
add_library(${LIBAMETSUCHI_NAME} STATIC
        ${AMETSUCHI_INCLUDE}
        ${AMETSUCHI_SRC}
        )
target_link_libraries(${LIBAMETSUCHI_NAME}
        cpp_redis
        )
StrictMode(${LIBAMETSUCHI_NAME})


if(TESTING)
    # Connect test executable and CMake test system.
    enable_testing()
    add_subdirectory(test)
endif(TESTING)



