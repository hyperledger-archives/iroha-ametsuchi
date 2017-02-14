# Compile the specified target as a modern, strict C++.
function(StrictMode target)
  # Require pure C++14 standard.
  set_target_properties(${target} PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
    )
  # Enable more warnings and turn them into compile errors.
  if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_CLANGCXX)
    target_compile_options(${target} PRIVATE
      -Wall
      -Wextra
      -Werror
      )
  elseif(CMAKE_COMPILER_IS_MSVCCXX OR CMAKE_COMPILER_IS_INTELCXX)
    target_compile_options(${target} PRIVATE
      /W3
      /WX
      )
  else()
    message(AUTHOR_WARNING
      "Unknown compiler: building target ${target} with default options"
      )
  endif()
endfunction()


# Creates test "test_name", with "SOURCES" (use string as second argument)
function(AddTest test_name SOURCES)
  list(APPEND SOURCES main.cc)
  add_executable(${test_name} ${SOURCES})
  add_dependencies(${test_name} gtest)
  target_include_directories(${test_name} PRIVATE ${gtest_INCLUDE_DIR})
  target_link_libraries(${test_name} PRIVATE ${gtest_LIBRARIES} pthread)
  add_test(
    NAME ${test_name}
    COMMAND $<TARGET_FILE:${test_name}>
  )
  StrictMode(${test_name})
endfunction()


# Creates benchmark "bench_name", with "SOURCES" (use string as second argument)
function(AddBenchmark bench_name SOURCES)
  add_executable(${bench_name} ${SOURCES})
  add_dependencies(${bench_name} gbenchmark)
  target_include_directories(${bench_name} PRIVATE ${gbenchmark_INCLUDE_DIR})
  target_link_libraries(${bench_name} PRIVATE ${gtest_LIBRARIES} pthread)
  StrictMode(${bench_name})
endfunction()
