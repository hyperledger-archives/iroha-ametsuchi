# Tests.
add_executable(testametsuchi
  main.cc
)

# Test dependencies.
ExternalProject_Add(catch
  GIT_REPOSITORY "https://github.com/philsquared/Catch.git"
  GIT_TAG "v1.7.0"
  SOURCE_DIR "${CMAKE_BINARY_DIR}/catch"
  CONFIGURE_COMMAND ""
  BUILD_COMMAND     ""
  INSTALL_COMMAND   ""
  TEST_COMMAND      ""
)


target_include_directories(testametsuchi
  PRIVATE "${CMAKE_BINARY_DIR}/catch/single_include"
)
add_dependencies(testametsuchi catch)
target_link_libraries(testametsuchi
  PRIVATE libametsuchi
)
add_dependencies(${AMETSUCHI_NAME} spdlog flatbuffers)

