# Download all external projects in the specified directory.
include(ExternalProject)
set(EP_PREFIX "${PROJECT_SOURCE_DIR}/external")
set_directory_properties(PROPERTIES 
  EP_PREFIX ${EP_PREFIX}
)

# Project dependencies.

################################
#         flatbuffers          #
################################
ExternalProject_Add(flatbuffers
  GIT_REPOSITORY    "https://github.com/google/flatbuffers.git"
  GIT_TAG           "v1.5.0"
  CONFIGURE_COMMAND cmake ${EP_PREFIX}/src/flatbuffers -DFLATBUFFERS_BUILD_TESTS=Off -DFLATBUFFERS_INSTALL=Off -DFLATBUFFERS_BUILD_FLATHASH=Off
  INSTALL_COMMAND   "" # remove install step
  TEST_COMMAND      "" # remove test step
  UPDATE_COMMAND    "" # remove update step
)
ExternalProject_Get_Property(flatbuffers source_dir)
set(flatbuffers_SOURCE_DIR "${source_dir}")
ExternalProject_Get_Property(flatbuffers binary_dir)
set(flatbuffers_BINARY_DIR "${binary_dir}")


#############################
#         speedlog          #
#############################
ExternalProject_Add(spdlog
  GIT_REPOSITORY    "https://github.com/gabime/spdlog.git"
  GIT_TAG           "v0.11.0"
  INSTALL_COMMAND   "" # remove install step
  TEST_COMMAND      "" # remove test step
  UPDATE_COMMAND    "" # remove update step
)
ExternalProject_Get_Property(spdlog source_dir)
set(spdlog_SOURCE_DIR "${source_dir}")



###########################
#         keccak          #
###########################
ExternalProject_Add(keccak
  GIT_REPOSITORY    "https://github.com/gvanas/KeccakCodePackage.git"
  CONFIGURE_COMMAND ""
  BUILD_IN_SOURCE   1
  BUILD_COMMAND     make && make generic64/libkeccak.a
  INSTALL_COMMAND   "" # remove install step
  TEST_COMMAND      "" # remove test step
  UPDATE_COMMAND    "" # remove update step
)
ExternalProject_Get_Property(keccak source_dir)
set(keccak_SOURCE_DIR  "${source_dir}")
set(keccak_INCLUDE_DIR ${keccak_SOURCE_DIR}/bin/generic64/libkeccak.a.headers)
set(keccak_LINK_DIR    ${keccak_SOURCE_DIR}/bin/generic64)
