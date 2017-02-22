# Download all external projects in the specified directory.
include(ExternalProject)
set(EP_PREFIX "${PROJECT_SOURCE_DIR}/external")
set_directory_properties(PROPERTIES
  EP_PREFIX ${EP_PREFIX}
  )

# Project dependencies.
find_package(Threads REQUIRED)

################################
#         flatbuffers          #
################################
find_package(Git REQUIRED)
set(flatbuffers_CMAKE_ARGS
  -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
  -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
  -DFLATBUFFERS_BUILD_TESTS=OFF
  -DFLATBUFFERS_INSTALL=OFF
  -DFLATBUFFERS_BUILD_FLATHASH=OFF
  -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
  )
if (NOT SCHEMA)
  list(APPEND flatbuffers_CMAKE_ARGS
    -DFLATBUFFERS_BUILD_FLATC=OFF
    )
endif ()
ExternalProject_Add(googleflatbuffers
  GIT_REPOSITORY    "https://github.com/google/flatbuffers.git"
  CMAKE_ARGS        ${flatbuffers_CMAKE_ARGS}
  INSTALL_COMMAND   "" # remove install step
  TEST_COMMAND      "" # remove test step
  UPDATE_COMMAND    "" # remove update step
  PATCH_COMMAND     ${GIT_EXECUTABLE} apply ${CMAKE_CURRENT_SOURCE_DIR}/patches/flatbuffers-Remove-libc-and-c-abi-requirement-on-Linux-with-clan.patch
  )
ExternalProject_Get_Property(googleflatbuffers source_dir binary_dir)
set(flatbuffers_SOURCE_DIR "${source_dir}")
set(flatbuffers_BINARY_DIR "${binary_dir}")

add_library(flatbuffers INTERFACE)
target_include_directories(flatbuffers INTERFACE ${flatbuffers_SOURCE_DIR}/include)
target_link_libraries(flatbuffers INTERFACE ${flatbuffers_BINARY_DIR}/libflatbuffers.a)
add_dependencies(flatbuffers googleflatbuffers)


#############################
#         speedlog          #
#############################
ExternalProject_Add(gabimespdlog
  GIT_REPOSITORY    "https://github.com/gabime/spdlog.git"
  CONFIGURE_COMMAND "" # remove configure step
  BUILD_COMMAND     "" # remove build step
  INSTALL_COMMAND   "" # remove install step
  TEST_COMMAND      "" # remove test step
  UPDATE_COMMAND    "" # remove update step
  )
ExternalProject_Get_Property(gabimespdlog source_dir)
set(spdlog_SOURCE_DIR "${source_dir}")

add_library(spdlog INTERFACE)
target_include_directories(spdlog INTERFACE ${spdlog_SOURCE_DIR}/include)
add_dependencies(spdlog gabimespdlog)


###########################
#         keccak          #
###########################
ExternalProject_Add(gvanaskeccak
  GIT_REPOSITORY    "https://github.com/gvanas/KeccakCodePackage.git"
  CONFIGURE_COMMAND ""
  BUILD_IN_SOURCE   1
  BUILD_COMMAND     $(MAKE) && $(MAKE) generic64/libkeccak.a
  INSTALL_COMMAND   "" # remove install step
  TEST_COMMAND      "" # remove test step
  UPDATE_COMMAND    "" # remove update step
  )
ExternalProject_Get_Property(gvanaskeccak source_dir)
set(keccak_SOURCE_DIR "${source_dir}")

add_library(keccak INTERFACE)
target_include_directories(keccak INTERFACE ${keccak_SOURCE_DIR}/bin/generic64/libkeccak.a.headers)
target_link_libraries(keccak INTERFACE ${keccak_SOURCE_DIR}/bin/generic64/libkeccak.a)
add_dependencies(keccak gvanaskeccak)


if(TESTING)
  ##########################
  #         gtest          #
  ##########################
  ExternalProject_Add(googletest
    GIT_REPOSITORY    "https://github.com/google/googletest.git"
    CMAKE_ARGS        -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
                      -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
                      -DGTEST_FORCE_SHARED_CRT=ON
                      -DGTEST_DISABLE_PTHREADS=OFF
                      -DBUILD_GTEST=ON
                      -DBUILD_GMOCK=OFF
                      -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
    INSTALL_COMMAND   "" # remove install step
    UPDATE_COMMAND    "" # remove update step
    TEST_COMMAND      "" # remove test step
    )
  ExternalProject_Get_Property(googletest source_dir binary_dir)
  set(gtest_SOURCE_DIR ${source_dir})
  set(gtest_BINARY_DIR ${binary_dir})

  add_library(gtest INTERFACE)
  target_include_directories(gtest INTERFACE ${gtest_SOURCE_DIR}/googletest/include)
  target_link_libraries(gtest INTERFACE ${gtest_BINARY_DIR}/googletest/libgtest.a)
  add_dependencies(gtest googletest)
endif(TESTING)


# BENCHMARKING is an option
if(BENCHMARKING)
  ##############################
  #         benchmark          #
  ##############################
  ExternalProject_Add(googlebenchmark
    GIT_REPOSITORY    "https://github.com/google/benchmark.git"
    CMAKE_ARGS        -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
                      -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
                      -DBENCHMARK_ENABLE_TESTING=OFF
                      -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
    INSTALL_COMMAND   "" # remove install step
    UPDATE_COMMAND    "" # remove update step
    TEST_COMMAND      "" # remove test step
    )
  ExternalProject_Get_Property(googlebenchmark source_dir binary_dir)
  set(benchmark_SOURCE_DIR ${source_dir})
  set(benchmark_BINARY_DIR ${binary_dir})

  add_library(benchmark INTERFACE)
  target_include_directories(benchmark INTERFACE ${benchmark_SOURCE_DIR}/include)
  target_link_libraries(benchmark INTERFACE ${benchmark_BINARY_DIR}/src/libbenchmark.a)
  add_dependencies(benchmark googlebenchmark)
endif(BENCHMARKING)
