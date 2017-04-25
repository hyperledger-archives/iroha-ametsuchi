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
#find_package(flatbuffers 1.6.0)

#if(NOT flatbuffers_FOUND)
  set(flatbuffers_CMAKE_ARGS
    -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
    -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
    -DFLATBUFFERS_BUILD_TESTS=OFF
    -DFLATBUFFERS_INSTALL=OFF
    -DFLATBUFFERS_BUILD_FLATHASH=OFF
    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
    -DFLATBUFFERS_BUILD_FLATC=ON
    )
ExternalProject_Add(google_flatbuffers
  GIT_REPOSITORY    "https://github.com/google/flatbuffers.git"
  CMAKE_ARGS        ${flatbuffers_CMAKE_ARGS}
  INSTALL_COMMAND   "" # remove install step
  TEST_COMMAND      "" # remove test step
  UPDATE_COMMAND    "" # remove update step
  )
ExternalProject_Get_Property(google_flatbuffers source_dir binary_dir)
set(flatbuffers_INCLUDE_DIRS ${source_dir}/include)
set(flatbuffers_LIBRARIES ${binary_dir}/libflatbuffers.a)
set(flatc_EXECUTABLE ${binary_dir}/flatc)
file(MAKE_DIRECTORY ${flatbuffers_INCLUDE_DIRS})

#endif ()

add_library(flatbuffers STATIC IMPORTED)
set_target_properties(flatbuffers PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES ${flatbuffers_INCLUDE_DIRS}
  IMPORTED_LOCATION ${flatbuffers_LIBRARIES}
  )

function(compile_fbs_to_cpp FBS)
  string(REGEX REPLACE "\\.fbs$" "_generated.h" GEN_HEADER ${FBS})
  add_custom_command(
    OUTPUT ${IROHA_SCHEMA_DIR}/${GEN_HEADER}
    COMMAND "${flatc_EXECUTABLE}" -c --scoped-enums --no-prefix --gen-mutable
  -o ${IROHA_SCHEMA_DIR} ${IROHA_SCHEMA_DIR}/${FBS}
    DEPENDS flatbuffers google_flatbuffers)
endfunction()

compile_fbs_to_cpp(account.fbs)
compile_fbs_to_cpp(asset.fbs)
compile_fbs_to_cpp(commands.fbs)
compile_fbs_to_cpp(main.fbs)
compile_fbs_to_cpp(primitives.fbs)
compile_fbs_to_cpp(transaction.fbs)

if(NOT flatbuffers_FOUND)
  add_dependencies(flatbuffers google_flatbuffers)
endif()




#############################
#         speedlog          #
#############################
find_package(spdlog 0.13.0)

if(NOT spdlog_FOUND)
  ExternalProject_Add(gabime_spdlog
    GIT_REPOSITORY    "https://github.com/gabime/spdlog.git"
    GIT_TAG           "v0.13.0"
    CONFIGURE_COMMAND "" # remove configure step
    BUILD_COMMAND     "" # remove build step
    INSTALL_COMMAND   "" # remove install step
    TEST_COMMAND      "" # remove test step
    UPDATE_COMMAND    "" # remove update step
    )
ExternalProject_Get_Property(gabime_spdlog source_dir)
set(spdlog_INCLUDE_DIRS ${source_dir}/include)
file(MAKE_DIRECTORY ${spdlog_INCLUDE_DIRS})
endif()

add_library(spdlog INTERFACE IMPORTED)
set_target_properties(spdlog PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES ${spdlog_INCLUDE_DIRS}
  )

if(TARGET gabime_spdlog)
  add_dependencies(spdlog gabime_spdlog)
endif()


###########################
#         keccak          #
###########################
find_package(keccak)

if(NOT keccak_FOUND)
  find_package(LibXslt QUIET)
  if (NOT LIBXSLT_XSLTPROC_EXECUTABLE)
    message(FATAL_ERROR "xsltproc not found")
  endif ()

  ExternalProject_Add(gvanas_keccak
    GIT_REPOSITORY    "https://github.com/gvanas/KeccakCodePackage.git"
    CONFIGURE_COMMAND ""
    BUILD_IN_SOURCE   1
    BUILD_COMMAND     CFLAGS="-fPIC" $(MAKE) generic64/libkeccak.a CC="${CMAKE_C_COMPILER}"
    INSTALL_COMMAND   "" # remove install step
    TEST_COMMAND      "" # remove test step
    UPDATE_COMMAND    "" # remove update step
    )
  ExternalProject_Get_Property(gvanas_keccak source_dir)
  set(keccak_INCLUDE_DIRS ${source_dir}/bin/generic64/libkeccak.a.headers)
  set(keccak_LIBRARIES ${source_dir}/bin/generic64/libkeccak.a)
  file(MAKE_DIRECTORY ${keccak_INCLUDE_DIRS})
endif()

add_library(keccak STATIC IMPORTED)
set_target_properties(keccak PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES ${keccak_INCLUDE_DIRS}
  IMPORTED_LOCATION ${keccak_LIBRARIES}
  IMPORTED_LINK_INTERFACE_LANGUAGES "C"
  )

if(NOT keccak_FOUND)
  add_dependencies(keccak gvanas_keccak)
endif()




###########################
#         LMDB            #
###########################
#find_package(LMDB)

if(NOT LMDB_FOUND)
  ExternalProject_Add(lmdb_LMDB
    GIT_REPOSITORY    "https://github.com/LMDB/lmdb.git"
    GIT_TAG           "LMDB_0.9.19"
    CONFIGURE_COMMAND ""
    BUILD_IN_SOURCE   1
    BUILD_COMMAND     cd libraries/liblmdb && $(MAKE) liblmdb.a  CC="${CMAKE_C_COMPILER}" "OPT=-fPIC -O3"
    INSTALL_COMMAND   "" # remove install step
    TEST_COMMAND      "" # remove test step
    UPDATE_COMMAND    "" # remove update step
    )
  ExternalProject_Get_Property(lmdb_LMDB source_dir)
  set(LMDB_INCLUDE_DIRS ${source_dir}/libraries/liblmdb)
  set(LMDB_LIBRARIES ${source_dir}/libraries/liblmdb/liblmdb.a)
  file(MAKE_DIRECTORY ${LMDB_INCLUDE_DIRS})
endif()

add_library(LMDB STATIC IMPORTED)
set_target_properties(LMDB PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES ${LMDB_INCLUDE_DIRS}
  IMPORTED_LOCATION ${LMDB_LIBRARIES}
  IMPORTED_LINK_INTERFACE_LANGUAGES "C"
  )

if(NOT LMDB_FOUND)
  add_dependencies(LMDB lmdb_LMDB)
endif()



if(TESTING)
  ##########################
  #         gtest          #
  ##########################
  find_package(gtest)

  if(NOT gtest_FOUND)
    ExternalProject_Add(google_test
      GIT_REPOSITORY    "https://github.com/google/googletest.git"
      CMAKE_ARGS        -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
                        -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
                        -Dgtest_force_shared_crt=ON
                        -Dgtest_disable_pthreads=OFF
                        -DBUILD_GTEST=ON
                        -DBUILD_GMOCK=OFF
                        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
      INSTALL_COMMAND   "" # remove install step
      UPDATE_COMMAND    "" # remove update step
      TEST_COMMAND      "" # remove test step
      )
    ExternalProject_Get_Property(google_test source_dir binary_dir)
    set(gtest_INCLUDE_DIRS ${source_dir}/googletest/include)
    set(gtest_LIBRARIES ${binary_dir}/googletest/libgtest.a)
    set(gtest_MAIN_LIBRARIES ${binary_dir}/googletest/libgtest_main.a)
    file(MAKE_DIRECTORY ${gtest_INCLUDE_DIRS})
  endif()

  add_library(gtest STATIC IMPORTED)
  set_target_properties(gtest PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES ${gtest_INCLUDE_DIRS}
    IMPORTED_LOCATION ${gtest_LIBRARIES}
    IMPORTED_LINK_INTERFACE_LIBRARIES "pthread;${gtest_MAIN_LIBRARIES}"
    )

  if(NOT gtest_FOUND)
    add_dependencies(gtest google_test)
  endif()

endif(TESTING)


# BENCHMARKING is an option
if(BENCHMARKING)
  ##############################
  #         benchmark          #
  ##############################
  find_package(benchmark)

  if(NOT benchmark_FOUND)
    ExternalProject_Add(google_benchmark
      GIT_REPOSITORY    "https://github.com/google/benchmark.git"
      CMAKE_ARGS        -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
                        -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
                        -DBENCHMARK_ENABLE_TESTING=OFF
                        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
      INSTALL_COMMAND   "" # remove install step
      UPDATE_COMMAND    "" # remove update step
      TEST_COMMAND      "" # remove test step
      )
    ExternalProject_Get_Property(google_benchmark source_dir binary_dir)
    set(benchmark_INCLUDE_DIRS ${source_dir}/include)
    set(benchmark_LIBRARIES ${binary_dir}/src/libbenchmark.a)
    file(MAKE_DIRECTORY ${benchmark_INCLUDE_DIRS})
  endif()

  add_library(benchmark STATIC IMPORTED)
  set_target_properties(benchmark PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES ${benchmark_INCLUDE_DIRS}
    IMPORTED_LOCATION ${benchmark_LIBRARIES}
    IMPORTED_LINK_INTERFACE_LIBRARIES "pthread"
    )

  if(NOT benchmark_FOUND)
    add_dependencies(benchmark google_benchmark)
  endif()

endif(BENCHMARKING)
