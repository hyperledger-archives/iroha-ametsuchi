# Download all external projects in the specified directory.
include(ExternalProject)
set(EP_PREFIX "${PROJECT_SOURCE_DIR}/external")
set_directory_properties(PROPERTIES
  EP_PREFIX ${EP_PREFIX}
)

include(cmake/DownloadProject.cmake)

# Project dependencies.
find_package(Threads REQUIRED)

################################
#         flatbuffers          #
################################
download_project(PROJ flatbuffers
  GIT_REPOSITORY      https://github.com/google/flatbuffers.git
  GIT_TAG             v1.5.0
  PREFIX              ${EP_PREFIX}
  )

set(FLATBUFFERS_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(FLATBUFFERS_INSTALL OFF CACHE BOOL "" FORCE)
set(FLATBUFFERS_BUILD_FLATHASH OFF CACHE BOOL "" FORCE)
if (NOT SCHEMA)
  set(FLATBUFFERS_BUILD_FLATC OFF CACHE BOOL "" FORCE)
endif ()
add_subdirectory(${flatbuffers_SOURCE_DIR} ${flatbuffers_BINARY_DIR})


#############################
#         speedlog          #
#############################
download_project(PROJ spdlog
  GIT_REPOSITORY      https://github.com/gabime/spdlog.git
  GIT_TAG             v0.11.0
  PREFIX              ${EP_PREFIX}
  )

set(SPDLOG_BUILD_TESTING OFF CACHE BOOL "" FORCE)
add_subdirectory(${spdlog_SOURCE_DIR} ${spdlog_BINARY_DIR})


###########################
#         keccak          #
###########################
find_package(LibXslt QUIET)
if (NOT LIBXSLT_XSLTPROC_EXECUTABLE)
  message(FATAL_ERROR "xsltproc not found")
endif ()

download_project(PROJ keccak
  GIT_REPOSITORY      https://github.com/gvanas/KeccakCodePackage.git
  GIT_TAG             master
  PREFIX              ${EP_PREFIX}
  )

execute_process(
  COMMAND make clean
  WORKING_DIRECTORY "${keccak_SOURCE_DIR}"
)
execute_process(
  COMMAND make
  WORKING_DIRECTORY "${keccak_SOURCE_DIR}"
  )
execute_process(
  COMMAND make generic64/libkeccak.a
  WORKING_DIRECTORY "${keccak_SOURCE_DIR}"
  )

add_library(keccak STATIC IMPORTED)
set_target_properties(keccak PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES ${keccak_SOURCE_DIR}/bin/generic64/libkeccak.a.headers
  IMPORTED_LOCATION ${keccak_SOURCE_DIR}/bin/generic64/libkeccak.a
  )


if(TESTING)
  ##########################
  #         gtest          #
  ##########################
  download_project(PROJ gtest
    GIT_REPOSITORY      https://github.com/google/googletest.git
    GIT_TAG             master
    PREFIX              ${EP_PREFIX}
    )

  set(GTEST_FORCE_SHARED_CRT ON CACHE BOOL "" FORCE)
  set(GTEST_DISABLE_PTHREADS OFF CACHE BOOL "" FORCE)
  set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
  set(BUILD_GTEST ON CACHE BOOL "" FORCE)
  add_subdirectory(${gtest_SOURCE_DIR} ${gtest_BINARY_DIR})
endif(TESTING)


# BENCHMARKING is an option
if(BENCHMARKING)
  ##############################
  #         benchmark          #
  ##############################
  download_project(PROJ benchmarking
    GIT_REPOSITORY      https://github.com/google/benchmark.git
    GIT_TAG             master
    PREFIX              ${EP_PREFIX}
    )

  set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
  add_subdirectory(${benchmarking_SOURCE_DIR} ${benchmarking_BINARY_DIR})
endif(BENCHMARKING)
