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
    externalproject_add(google_test
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
    externalproject_get_property(google_test source_dir binary_dir)
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
externalproject_add(cylix_cpp_redis
    GIT_REPOSITORY "https://github.com/Cylix/cpp_redis.git"
    CMAKE_ARGS -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
    -DCMAKE_POSITION_INDEPENDENT_CODE=ON
    INSTALL_COMMAND "" # remove install step
    UPDATE_COMMAND "" # remove update step
    TEST_COMMAND "" # remove test step
    )
externalproject_get_property(cylix_cpp_redis source_dir binary_dir)
set(CPP_REDIS_INCLUDE_DIRS ${source_dir}/includes)
set(CPP_REDIS_LIBRARIES ${binary_dir}/lib/libcpp_redis.a)
set(TACOPIE_LIBRARIES ${binary_dir}/lib/libtacopie.a)
set(TACOPIE_OBJECTS
    ${binary_dir}/tacopie/CMakeFiles/tacopie.dir/__/sources/logger.cpp.o
    ${binary_dir}/tacopie/CMakeFiles/tacopie.dir/__/sources/reply.cpp.o
    ${binary_dir}/tacopie/CMakeFiles/tacopie.dir/__/sources/redis_client.cpp.o
    ${binary_dir}/tacopie/CMakeFiles/tacopie.dir/__/sources/future_client.cpp.o
    ${binary_dir}/tacopie/CMakeFiles/tacopie.dir/__/sources/redis_subscriber.cpp.o
    ${binary_dir}/tacopie/CMakeFiles/tacopie.dir/__/sources/network/redis_connection.cpp.o
    ${binary_dir}/tacopie/CMakeFiles/tacopie.dir/__/sources/builders/reply_builder.cpp.o
    ${binary_dir}/tacopie/CMakeFiles/tacopie.dir/__/sources/builders/array_builder.cpp.o
    ${binary_dir}/tacopie/CMakeFiles/tacopie.dir/__/sources/builders/builders_factory.cpp.o
    ${binary_dir}/tacopie/CMakeFiles/tacopie.dir/__/sources/builders/bulk_string_builder.cpp.o
    ${binary_dir}/tacopie/CMakeFiles/tacopie.dir/__/sources/builders/simple_string_builder.cpp.o
    ${binary_dir}/tacopie/CMakeFiles/tacopie.dir/__/sources/builders/integer_builder.cpp.o
    ${binary_dir}/tacopie/CMakeFiles/tacopie.dir/__/sources/builders/error_builder.cpp.o
    ${binary_dir}/tacopie/CMakeFiles/tacopie.dir/sources/error.cpp.o
    ${binary_dir}/tacopie/CMakeFiles/tacopie.dir/sources/logger.cpp.o
    ${binary_dir}/tacopie/CMakeFiles/tacopie.dir/sources/network/tcp_server.cpp.o
    ${binary_dir}/tacopie/CMakeFiles/tacopie.dir/sources/network/tcp_client.cpp.o
    ${binary_dir}/tacopie/CMakeFiles/tacopie.dir/sources/utils/thread_pool.cpp.o
    ${binary_dir}/tacopie/CMakeFiles/tacopie.dir/sources/network/unix/io_service.cpp.o
    ${binary_dir}/tacopie/CMakeFiles/tacopie.dir/sources/network/unix/self_pipe.cpp.o
    ${binary_dir}/tacopie/CMakeFiles/tacopie.dir/sources/network/unix/tcp_socket.cpp.o
    )
set(CPP_REDIS_OBJECTS
    ${binary_dir}/CMakeFiles/cpp_redis.dir/sources/logger.cpp.o
    ${binary_dir}/CMakeFiles/cpp_redis.dir/sources/reply.cpp.o
    ${binary_dir}/CMakeFiles/cpp_redis.dir/sources/redis_client.cpp.o
    ${binary_dir}/CMakeFiles/cpp_redis.dir/sources/future_client.cpp.o
    ${binary_dir}/CMakeFiles/cpp_redis.dir/sources/redis_subscriber.cpp.o
    ${binary_dir}/CMakeFiles/cpp_redis.dir/sources/network/redis_connection.cpp.o
    ${binary_dir}/CMakeFiles/cpp_redis.dir/sources/builders/reply_builder.cpp.o
    ${binary_dir}/CMakeFiles/cpp_redis.dir/sources/builders/array_builder.cpp.o
    ${binary_dir}/CMakeFiles/cpp_redis.dir/sources/builders/builders_factory.cpp.o
    ${binary_dir}/CMakeFiles/cpp_redis.dir/sources/builders/bulk_string_builder.cpp.o
    ${binary_dir}/CMakeFiles/cpp_redis.dir/sources/builders/simple_string_builder.cpp.o
    ${binary_dir}/CMakeFiles/cpp_redis.dir/sources/builders/integer_builder.cpp.o
    ${binary_dir}/CMakeFiles/cpp_redis.dir/sources/builders/error_builder.cpp.o
    )

##########################
#          pqxx          #
##########################
find_package(PostgreSQL QUIET)
if (NOT PostgreSQL_LIBRARY)
  message(FATAL_ERROR "libpq not found")
endif ()

externalproject_add(jtv_libpqxx
    GIT_REPOSITORY "https://github.com/jtv/libpqxx.git"
    CONFIGURE_COMMAND ./configure --disable-documentation --with-pic
    BUILD_IN_SOURCE 1
    BUILD_COMMAND $(MAKE)
    INSTALL_COMMAND "" # remove install step
    TEST_COMMAND "" # remove test step
    UPDATE_COMMAND "" # remove update step
    )
externalproject_get_property(jtv_libpqxx source_dir)
set(pqxx_INCLUDE_DIRS ${source_dir}/include)
set(pqxx_LIBRARIES ${source_dir}/src/.libs/libpqxx.a)
set(pqxx_OBJECTS
    ${source_dir}/src/binarystring.o
    ${source_dir}/src/connection_base.o
    ${source_dir}/src/connection.o
    ${source_dir}/src/cursor.o
    ${source_dir}/src/dbtransaction.o
    ${source_dir}/src/errorhandler.o
    ${source_dir}/src/except.o
    ${source_dir}/src/field.o
    ${source_dir}/src/largeobject.o
    ${source_dir}/src/nontransaction.o
    ${source_dir}/src/notification.o
    ${source_dir}/src/pipeline.o
    ${source_dir}/src/prepared_statement.o
    ${source_dir}/src/result.o
    ${source_dir}/src/robusttransaction.o
    ${source_dir}/src/statement_parameters.o
    ${source_dir}/src/strconv.o
    ${source_dir}/src/subtransaction.o
    ${source_dir}/src/tablereader.o
    ${source_dir}/src/tablestream.o
    ${source_dir}/src/tablewriter.o
    ${source_dir}/src/transaction.o
    ${source_dir}/src/transaction_base.o
    ${source_dir}/src/row.o
    ${source_dir}/src/util.o
    )