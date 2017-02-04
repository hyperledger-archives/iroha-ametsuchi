# Tests.
add_executable(testametsuchi
  main.cc
)

# Test dependencies.
ExternalProject_Add(catch
  GIT_REPOSITORY "https://github.com/philsquared/Catch.git"
  GIT_TAG "v1.7.0"
  SOURCE_DIR "${CMAKE_BINARY_DIR}/catch"
  CONFIGURE_COMMAND "" # remove configure step
  BUILD_COMMAND     "" # remove build step
  INSTALL_COMMAND   "" # remove install step
  TEST_COMMAND      "" # remove test step
  UPDATE_COMMAND    "" # remove update step
)


target_include_directories(testametsuchi
  PRIVATE "${CMAKE_BINARY_DIR}/catch/single_include"
)
add_dependencies(testametsuchi catch)
target_link_libraries(testametsuchi
  PRIVATE ${LIBAMETSUCHI_NAME}
)

