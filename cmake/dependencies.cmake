include(ExternalProject)
set(EP_PREFIX "${PROJECT_SOURCE_DIR}/external")
set_directory_properties(PROPERTIES
        EP_PREFIX ${EP_PREFIX}
        )

if(TESTING)
    ##########################
    #         gtest          #
    ##########################
    find_package(GTest)

    if(NOT gtest_FOUND)
        ExternalProject_Add(google_test
                GIT_REPOSITORY    "https://github.com/google/googletest.git"
                CMAKE_ARGS        -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
                -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
                -Dgtest_force_shared_crt=ON
                -Dgtest_disable_pthreads=OFF
                -DBUILD_GTEST=ON
                -DBUILD_GMOCK=OFF
                -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                INSTALL_COMMAND   "" # remove install step
                UPDATE_COMMAND    "" # remove update step
                TEST_COMMAND      "" # remove test step
                )
        ExternalProject_Get_Property(google_test source_dir binary_dir)
        set(gtest_INCLUDE_DIRS ${source_dir}/googletest/include)
        set(gtest_LIBRARIES ${binary_dir}/googletest/libgtest.a)
        set(gtest_MAIN_LIBRARIES ${binary_dir}/googletest/libgtest_main.a)
        file(MAKE_DIRECTORY ${gtest_INCLUDE_DIRS})
    endif()

    add_library(gtest STATIC IMPORTED)
    set_target_properties(gtest PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES ${gtest_INCLUDE_DIRS}
            IMPORTED_LOCATION ${gtest_LIBRARIES}
            IMPORTED_LINK_INTERFACE_LIBRARIES "pthread;${gtest_MAIN_LIBRARIES}"
            )

    if(NOT gtest_FOUND)
        add_dependencies(gtest google_test)
    endif()

endif(TESTING)

ExternalProject_Add(cylix_cpp_redis
        GIT_REPOSITORY "https://github.com/Cylix/cpp_redis.git"
        CMAKE_ARGS -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
        -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
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
        IMPORTED_LINK_INTERFACE_LIBRARIES ${TACOPIE_LIBRARIES}
        )

add_dependencies(cpp_redis cylix_cpp_redis)