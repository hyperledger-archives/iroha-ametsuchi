# Compile the specified target as a modern, strict C++.
function(StrictMode target)
  # Require pure C++14 standard.
  set_target_properties(${target} PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
    )
  # Enable more warnings and turn them into compile errors.
  if ((CMAKE_CXX_COMPILER_ID STREQUAL "GNU") OR
  (CMAKE_CXX_COMPILER_ID STREQUAL "Clang") OR
  (CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang"))
    target_compile_options(${target} PRIVATE -Wall -Wpedantic)
  elseif ((CMAKE_CXX_COMPILER_ID STREQUAL "MSVC") OR
  (CMAKE_CXX_COMPILER_ID STREQUAL "Intel"))
    target_compile_options(${target} PRIVATE /W3 /WX)
  else ()
    message(AUTHOR_WARNING "Unknown compiler: building target ${target} with default options")
  endif ()
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
  string(REGEX REPLACE "\\.fbs$" "fbs_h" GEN_TARGET ${FBS})
  add_custom_command(
    OUTPUT ${IROHA_SCHEMA_DIR}/${GEN_HEADER}
    COMMAND "${flatc_EXECUTABLE}" -c --scoped-enums --no-prefix --gen-mutable
    -o ${IROHA_SCHEMA_DIR} ${IROHA_SCHEMA_DIR}/${FBS}
    DEPENDS flatc)
  add_custom_target(${GEN_TARGET} DEPENDS "${IROHA_SCHEMA_DIR}/${GEN_HEADER}")
  add_dependencies(flatbuffers ${GEN_TARGET})
endfunction()

function(compile_proto_to_cpp PROTO)
  string(REGEX REPLACE "\\.proto$" ".pb.h" GEN_PB_HEADER ${PROTO})
  string(REGEX REPLACE "\\.proto$" ".pb.cc" GEN_PB ${PROTO})
  string(REGEX REPLACE "\\.proto$" "proto_h" GEN_TARGET ${PROTO})
  add_custom_command(
    OUTPUT ${IROHA_SCHEMA_DIR}/${GEN_PB_HEADER} ${IROHA_SCHEMA_DIR}/${GEN_PB}
    COMMAND "${protoc_EXECUTABLE}" --cpp_out=${IROHA_SCHEMA_DIR} ${PROTO}
    DEPENDS protoc
    WORKING_DIRECTORY ${IROHA_SCHEMA_DIR})
  add_library(${GEN_TARGET}
    "${IROHA_SCHEMA_DIR}/${GEN_PB_HEADER}"
    "${IROHA_SCHEMA_DIR}/${GEN_PB}")
  target_include_directories(${GEN_TARGET} PUBLIC
    ${protobuf_INCLUDE_DIRS}
    )
  target_link_libraries(${GEN_TARGET}
    ${protobuf_LIBRARIES}
    )
  set_property(TARGET protobuf APPEND PROPERTY INTERFACE_LINK_LIBRARIES ${GEN_TARGET})
  add_dependencies(protobuf ${GEN_TARGET})
endfunction()
