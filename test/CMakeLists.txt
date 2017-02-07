# Test dependencies.
ExternalProject_Add(catch
  GIT_REPOSITORY    "https://github.com/philsquared/Catch.git"
  GIT_TAG           "v1.7.0"
  CONFIGURE_COMMAND "" # remove configure step
  BUILD_COMMAND     "" # remove build step
  INSTALL_COMMAND   "" # remove install step
  TEST_COMMAND      "" # remove test step
  UPDATE_COMMAND    "" # remove update step
)
ExternalProject_Get_Property(catch source_dir)
set(catch_SOURCE_DIR "${source_dir}")


# Dummy test
add_executable(testametsuchi
  main.cc
)

target_include_directories(testametsuchi
  PRIVATE "${catch_SOURCE_DIR}/single_include"
)
add_dependencies(testametsuchi catch)
target_link_libraries(testametsuchi
  PRIVATE ${LIBAMETSUCHI_NAME}
)


# List of tests:
add_test(testametsuchi testametsuchi)