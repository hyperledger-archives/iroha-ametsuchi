# Compile the specified target as a modern, strict C++.
function(StrictMode target)
  # Require pure C++14 standard.
  set_target_properties(${target} PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
  )
  # Enable more warnings and turn them into compile errors.
  if((CMAKE_CXX_COMPILER_ID STREQUAL "GNU") OR
     (CMAKE_CXX_COMPILER_ID STREQUAL "Clang") OR
     (CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang"))
    target_compile_options(${target} PRIVATE -Wall -Wpedantic)
  elseif((CMAKE_CXX_COMPILER_ID STREQUAL "MSVC") OR
         (CMAKE_CXX_COMPILER_ID STREQUAL "Intel"))
    target_compile_options(${target} PRIVATE /W3 /WX)
  else()
    message(AUTHOR_WARNING "Unknown compiler: building target ${target} with default options")
  endif()
endfunction()


# Creates test "test_name", with "SOURCES" (use string as second argument)
function(AddTest test_name SOURCES)
  add_executable(${test_name} ${SOURCES})
  target_link_libraries(${test_name} gtest)
  add_test(
    NAME ${test_name}
    COMMAND $<TARGET_FILE:${test_name}>
  )
  StrictMode(${test_name})
endfunction()


# Creates benchmark "bench_name", with "SOURCES" (use string as second argument)
function(AddBenchmark bench_name SOURCES)
  add_executable(${bench_name} ${SOURCES})
  target_link_libraries(${bench_name} PRIVATE benchmark)
  StrictMode(${bench_name})
endfunction()



function(compile_fbs_to_cpp FBS)
  string(REGEX REPLACE "\\.fbs$" "_generated.h" GEN_HEADER ${FBS})
  string(REGEX REPLACE "\\.fbs$" "_h" GEN_TARGET ${FBS})
  add_custom_command(
    OUTPUT ${IROHA_SCHEMA_DIR}/${GEN_HEADER}
    COMMAND "${flatc_EXECUTABLE}" -c --scoped-enums --no-prefix --gen-mutable
    -o ${IROHA_SCHEMA_DIR} ${IROHA_SCHEMA_DIR}/${FBS}
    DEPENDS flatc)
  add_custom_target(${GEN_TARGET} DEPENDS "${IROHA_SCHEMA_DIR}/${GEN_HEADER}")
  add_dependencies(flatbuffers ${GEN_TARGET})
endfunction()
