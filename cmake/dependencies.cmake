include(ExternalProject)
set(EP_PREFIX "${PROJECT_SOURCE_DIR}/external")
set_directory_properties(PROPERTIES
  EP_PREFIX ${EP_PREFIX}
  )

if (TESTING)
  ##########################
  #         gtest          #
  ##########################
  find_package(GTest)

  if (NOT GTEST_FOUND)
    ExternalProject_Add(google_test
      GIT_REPOSITORY "https://github.com/google/googletest.git"
      CMAKE_ARGS -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
      -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
      -Dgtest_force_shared_crt=ON
      -Dgtest_disable_pthreads=OFF
      -DBUILD_GTEST=ON
      -DBUILD_GMOCK=OFF
      -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
      INSTALL_COMMAND "" # remove install step
      UPDATE_COMMAND "" # remove update step
      TEST_COMMAND "" # remove test step
      )
    ExternalProject_Get_Property(google_test source_dir binary_dir)
    set(gtest_INCLUDE_DIRS ${source_dir}/googletest/include)
    set(gtest_LIBRARIES ${binary_dir}/googletest/libgtest.a)
    set(gtest_MAIN_LIBRARIES ${binary_dir}/googletest/libgtest_main.a)
    file(MAKE_DIRECTORY ${gtest_INCLUDE_DIRS})
  else ()
    set(gtest_INCLUDE_DIRS ${GTEST_INCLUDE_DIRS})
    set(gtest_LIBRARIES ${GTEST_LIBRARIES})
    set(gtest_MAIN_LIBRARIES ${GTEST_MAIN_LIBRARIES})
  endif ()

  add_library(gtest STATIC IMPORTED)
  set_target_properties(gtest PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES ${gtest_INCLUDE_DIRS}
    IMPORTED_LOCATION ${gtest_LIBRARIES}
    IMPORTED_LINK_INTERFACE_LIBRARIES "pthread;${gtest_MAIN_LIBRARIES}"
    )

  if (NOT GTEST_FOUND)
    add_dependencies(gtest google_test)
  endif ()

endif (TESTING)

##########################
#       cpp_redis        #
##########################
find_package(CPP_redis)
if (NOT CPP_redis_FOUND)
  ExternalProject_Add(cylix_cpp_redis
    GIT_REPOSITORY "https://github.com/Cylix/cpp_redis.git"
    CMAKE_ARGS -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
    -DCMAKE_POSITION_INDEPENDENT_CODE=ON
    INSTALL_COMMAND "" # remove install step
    UPDATE_COMMAND "" # remove update step
    TEST_COMMAND "" # remove test step
    )
  ExternalProject_Get_Property(cylix_cpp_redis source_dir binary_dir)
  set(CPP_REDIS_INCLUDE_DIRS ${source_dir}/includes)
  set(CPP_REDIS_LIBRARIES ${binary_dir}/lib/libcpp_redis.a)
  set(TACOPIE_LIBRARIES ${binary_dir}/lib/libtacopie.a)
  file(MAKE_DIRECTORY ${CPP_REDIS_INCLUDE_DIRS})

  add_library(cpp_redis STATIC IMPORTED)
  set_target_properties(cpp_redis PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES ${CPP_REDIS_INCLUDE_DIRS}
    IMPORTED_LOCATION ${CPP_REDIS_LIBRARIES}
    IMPORTED_LINK_INTERFACE_LIBRARIES "${TACOPIE_LIBRARIES};pthread"
    )

  add_dependencies(cpp_redis cylix_cpp_redis)
endif ()

##########################
#          pqxx          #
##########################
find_package(PQxx)
if (NOT PQxx_FOUND)
  find_package(PostgreSQL QUIET)
  if (NOT PostgreSQL_LIBRARY)
    message(FATAL_ERROR "libpq not found")
  endif ()

  ExternalProject_Add(jtv_libpqxx
    GIT_REPOSITORY "https://github.com/jtv/libpqxx.git"
    CONFIGURE_COMMAND ./configure --disable-documentation --with-pic
    BUILD_IN_SOURCE 1
    BUILD_COMMAND $(MAKE)
    INSTALL_COMMAND "" # remove install step
    TEST_COMMAND "" # remove test step
    UPDATE_COMMAND "" # remove update step
    )
  ExternalProject_Get_Property(jtv_libpqxx source_dir)
  set(pqxx_INCLUDE_DIRS ${source_dir}/include)
  set(pqxx_LIBRARIES ${source_dir}/src/.libs/libpqxx.a)
  file(MAKE_DIRECTORY ${pqxx_INCLUDE_DIRS})

  add_library(pqxx STATIC IMPORTED)
  set_target_properties(pqxx PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES ${pqxx_INCLUDE_DIRS}
    IMPORTED_LOCATION ${pqxx_LIBRARIES}
    INTERFACE_LINK_LIBRARIES "pq"
    )

  add_dependencies(pqxx jtv_libpqxx)
endif ()
