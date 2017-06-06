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

  if (NOT gtest_FOUND)
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
  endif ()

  add_library(gtest STATIC IMPORTED)
  set_target_properties(gtest PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES ${gtest_INCLUDE_DIRS}
    IMPORTED_LOCATION ${gtest_LIBRARIES}
    IMPORTED_LINK_INTERFACE_LIBRARIES "pthread;${gtest_MAIN_LIBRARIES}"
    )

  if (NOT gtest_FOUND)
    add_dependencies(gtest google_test)
  endif ()

endif (TESTING)

##########################
#       cpp_redis        #
##########################
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
  IMPORTED_LINK_INTERFACE_LIBRARIES ${TACOPIE_LIBRARIES}
  )

add_dependencies(cpp_redis cylix_cpp_redis)

###########################
#         keccak          #
###########################
find_package(LibXslt QUIET)
if (NOT LIBXSLT_XSLTPROC_EXECUTABLE)
  message(FATAL_ERROR "xsltproc not found")
endif ()

ExternalProject_Add(gvanas_keccak
  GIT_REPOSITORY "https://github.com/gvanas/KeccakCodePackage.git"
  CONFIGURE_COMMAND ""
  BUILD_IN_SOURCE 1
  BUILD_COMMAND CFLAGS="-fPIC" $(MAKE) generic64/libkeccak.a CC="${CMAKE_C_COMPILER}"
  INSTALL_COMMAND "" # remove install step
  TEST_COMMAND "" # remove test step
  UPDATE_COMMAND "" # remove update step
  )
ExternalProject_Get_Property(gvanas_keccak source_dir)
set(keccak_INCLUDE_DIRS ${source_dir}/bin/generic64/libkeccak.a.headers)
set(keccak_LIBRARIES ${source_dir}/bin/generic64/libkeccak.a)
file(MAKE_DIRECTORY ${keccak_INCLUDE_DIRS})

add_library(keccak STATIC IMPORTED)
set_target_properties(keccak PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES ${keccak_INCLUDE_DIRS}
  IMPORTED_LOCATION ${keccak_LIBRARIES}
  IMPORTED_LINK_INTERFACE_LANGUAGES "C"
  )

add_dependencies(keccak gvanas_keccak)

##########################
#        cppassist       #
##########################
ExternalProject_Add(cginternals_cppassist
  GIT_REPOSITORY "https://github.com/cginternals/cppassist.git"
  CMAKE_ARGS -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
  -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
  -DBUILD_SHARED_LIBS=OFF
  -DOPTION_BUILD_TESTS=OFF
  BUILD_IN_SOURCE 1
  INSTALL_COMMAND "" # remove install step
  UPDATE_COMMAND "" # remove update step
  TEST_COMMAND "" # remove test step
  )
ExternalProject_Get_Property(cginternals_cppassist source_dir)
set(CPPASSIST_SOURCE_DIR ${source_dir})
set(CPPASSIST_INCLUDE_DIRS ${source_dir}/source/cppassist/include)
set(CPPASSIST_LIBRARIES ${source_dir}/libcppassistd.a)
file(MAKE_DIRECTORY ${CPPASSIST_INCLUDE_DIRS})

add_library(cppassist STATIC IMPORTED)
set_target_properties(cppassist PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES ${CPPASSIST_INCLUDE_DIRS}
  IMPORTED_LOCATION ${CPPASSIST_LIBRARIES}
  )

add_dependencies(cppassist cginternals_cppassist)

##########################
#        cpplocate       #
##########################
ExternalProject_Add(cginternals_cpplocate
  GIT_REPOSITORY "https://github.com/cginternals/cpplocate.git"
  CMAKE_ARGS -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
  -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
  -DBUILD_SHARED_LIBS=OFF
  -DOPTION_BUILD_TESTS=OFF
  BUILD_IN_SOURCE 1
  INSTALL_COMMAND "" # remove install step
  UPDATE_COMMAND "" # remove update step
  TEST_COMMAND "" # remove test step
  )
ExternalProject_Get_Property(cginternals_cpplocate source_dir)
set(CPPLOCATE_SOURCE_DIR ${source_dir})
set(CPPLOCATE_INCLUDE_DIRS ${source_dir}/source/cpplocate/include)
set(CPPLOCATE_LIBRARIES ${source_dir}/libcpplocated.a)
file(MAKE_DIRECTORY ${CPPLOCATE_INCLUDE_DIRS})

add_library(cpplocate STATIC IMPORTED)
set_target_properties(cpplocate PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES ${CPPLOCATE_INCLUDE_DIRS}
  IMPORTED_LOCATION ${CPPLOCATE_LIBRARIES}
  )

add_dependencies(cpplocate cginternals_cpplocate)

##########################
#        cppexpose       #
##########################
ExternalProject_Add(cginternals_cppexpose
  GIT_REPOSITORY "https://github.com/cginternals/cppexpose.git"
  CMAKE_ARGS -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
  -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
  -DBUILD_SHARED_LIBS=OFF
  -DOPTION_BUILD_TESTS=OFF
  -Dcppassist_DIR=${CPPASSIST_SOURCE_DIR}
  -Dcpplocate_DIR=${CPPLOCATE_SOURCE_DIR}
  BUILD_IN_SOURCE 1
  INSTALL_COMMAND "" # remove install step
  UPDATE_COMMAND "" # remove update step
  TEST_COMMAND "" # remove test step
  )
ExternalProject_Get_Property(cginternals_cppexpose source_dir)
set(CPPEXPOSE_SOURCE_DIR ${source_dir})
add_dependencies(cginternals_cppexpose cppassist cpplocate)
set(CPPEXPOSE_INCLUDE_DIRS ${source_dir}/source/cppexpose/include)
set(CPPEXPOSE_LIBRARIES ${source_dir}/libcppexposed.a)
file(MAKE_DIRECTORY ${CPPEXPOSE_INCLUDE_DIRS})

add_library(cppexpose STATIC IMPORTED)
set_target_properties(cppexpose PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES "${CPPEXPOSE_INCLUDE_DIRS};${CPPASSIST_INCLUDE_DIRS};${CPPLOCATE_INCLUDE_DIRS}"
  IMPORTED_LOCATION ${CPPEXPOSE_LIBRARIES}
  INTERFACE_LINK_LIBRARIES "${CPPASSIST_LIBRARIES};${CPPLOCATE_LIBRARIES}"
  )

add_dependencies(cppexpose cginternals_cppexpose)

##########################
#         cppfs          #
##########################
ExternalProject_Add(cginternals_cppfs
  GIT_REPOSITORY "https://github.com/cginternals/cppfs.git"
  CMAKE_ARGS -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
  -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
  -DBUILD_SHARED_LIBS=OFF
  -DOPTION_BUILD_TESTS=OFF
  -Dcppexpose_DIR=${CPPEXPOSE_SOURCE_DIR}
  -Dcppassist_DIR=${CPPASSIST_SOURCE_DIR}
  -Dcpplocate_DIR=${CPPLOCATE_SOURCE_DIR}
  BUILD_IN_SOURCE 1
  INSTALL_COMMAND "" # remove install step
  UPDATE_COMMAND "" # remove update step
  TEST_COMMAND "" # remove test step
  )
add_dependencies(cginternals_cppfs cppexpose)
ExternalProject_Get_Property(cginternals_cppfs source_dir)
set(CPPFS_INCLUDE_DIRS ${source_dir}/source/cppfs/include)
set(CPPFS_LIBRARIES ${source_dir}/libcppfsd.a)
file(MAKE_DIRECTORY ${CPPFS_INCLUDE_DIRS})

add_library(cppfs STATIC IMPORTED)
set_target_properties(cppfs PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES ${CPPFS_INCLUDE_DIRS}
  IMPORTED_LOCATION ${CPPFS_LIBRARIES}
  INTERFACE_LINK_LIBRARIES "${CPPEXPOSE_LIBRARIES};${CPPASSIST_LIBRARIES};${CPPLOCATE_LIBRARIES};ssh2;ssl;crypto"
  )

add_dependencies(cppfs cginternals_cppfs)

##########################
#          pqxx          #
##########################
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

################################
#         flatbuffers          #
################################
find_package(Flatbuffers)
if (NOT Flatbuffers_FOUND OR NOT Flatbuffers_flatc_EXECUTABLE)
  set(flatbuffers_CMAKE_ARGS
    -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
    -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
    -DFLATBUFFERS_BUILD_TESTS=OFF
    -DFLATBUFFERS_INSTALL=OFF
    -DFLATBUFFERS_BUILD_FLATHASH=OFF
    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
    -DFLATBUFFERS_BUILD_FLATC=ON
    )
  ExternalProject_Add(google_flatbuffers
    GIT_REPOSITORY "https://github.com/google/flatbuffers.git"
    CMAKE_ARGS ${flatbuffers_CMAKE_ARGS}
    INSTALL_COMMAND "" # remove install step
    TEST_COMMAND "" # remove test step
    UPDATE_COMMAND "" # remove update step
    )
  ExternalProject_Get_Property(google_flatbuffers source_dir binary_dir)
  set(flatbuffers_INCLUDE_DIRS ${source_dir}/include)
  set(flatbuffers_LIBRARIES ${binary_dir}/libflatbuffers.a)
  set(flatc_EXECUTABLE ${binary_dir}/flatc)
  file(MAKE_DIRECTORY ${flatbuffers_INCLUDE_DIRS})
  add_custom_target(flatc DEPENDS google_flatbuffers)
else()
  set(flatbuffers_INCLUDE_DIRS ${Flatbuffers_INCLUDE_DIRS})
  set(flatbuffers_LIBRARIES ${Flatbuffers_LIBRARIES})
  set(flatc_EXECUTABLE ${Flatbuffers_flatc_EXECUTABLE})
  add_custom_target(flatc)
endif ()

add_library(flatbuffers STATIC IMPORTED)
set_target_properties(flatbuffers PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES ${flatbuffers_INCLUDE_DIRS}
  IMPORTED_LOCATION ${flatbuffers_LIBRARIES}
  )

if (NOT Flatbuffers_FOUND OR NOT Flatbuffers_flatc_EXECUTABLE)
  add_dependencies(flatbuffers google_flatbuffers flatc)
endif()

################################
#           protobuf           #
################################
find_package(Protobuf 3.0.0)
if (NOT PROTOBUF_FOUND OR NOT PROTOBUF_PROTOC_EXECUTABLE)
  ExternalProject_Add(google_protobuf
    URL https://github.com/google/protobuf/releases/download/v3.3.0/protobuf-cpp-3.3.0.tar.gz
    CONFIGURE_COMMAND ./configure
    BUILD_IN_SOURCE 1
    BUILD_COMMAND $(MAKE)
    INSTALL_COMMAND "" # remove install step
    TEST_COMMAND "" # remove test step
    UPDATE_COMMAND "" # remove update step
    )
  ExternalProject_Get_Property(google_protobuf source_dir)
  set(protobuf_INCLUDE_DIRS ${source_dir}/src)
  set(protobuf_LIBRARIES ${source_dir}/src/.libs/libprotobuf.a)
  set(protoc_EXECUTABLE ${source_dir}/src/protoc)
  file(MAKE_DIRECTORY ${protobuf_INCLUDE_DIRS})
  add_custom_target(protoc DEPENDS google_protobuf)
else()
  set(protobuf_INCLUDE_DIRS ${PROTOBUF_INCLUDE_DIRS})
  set(protobuf_LIBRARIES ${PROTOBUF_LIBRARY})
  set(protoc_EXECUTABLE ${PROTOBUF_PROTOC_EXECUTABLE})
  add_custom_target(protoc)
endif ()

add_library(protobuf STATIC IMPORTED)
set_target_properties(protobuf PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES ${protobuf_INCLUDE_DIRS}
  IMPORTED_LOCATION ${protobuf_LIBRARIES}
  )

if (NOT PROTOBUF_FOUND OR NOT PROTOBUF_PROTOC_EXECUTABLE)
  add_dependencies(protobuf google_protobuf protoc)
endif()

#########################
#         gRPC          #
#########################
find_package(GRPC)
if (NOT GRPC_FOUND)
  ExternalProject_Add(grpc_grpc
    GIT_REPOSITORY "https://github.com/grpc/grpc.git"
    GIT_TAG "v1.3.0"
    BUILD_IN_SOURCE 1
    BUILD_COMMAND $(MAKE)
    CONFIGURE_COMMAND "" # remove configure step
    INSTALL_COMMAND "" # remove install step
    TEST_COMMAND "" # remove test step
    UPDATE_COMMAND "" # remove update step
    )
  ExternalProject_Get_Property(grpc_grpc source_dir)
  set(grpc_INCLUDE_DIR ${source_dir}/include)
  set(grpc_LIBRARY ${source_dir}/libs/opt/libgrpc.so)
  set(grpc_grpc++_LIBRARY ${source_dir}/libs/opt/libgrpc++.so)
  set(grpc_grpc++_reflection_LIBRARY ${source_dir}/libs/opt/libgrpc++_reflection.so)
  set(grpc_CPP_PLUGIN ${source_dir})
  file(MAKE_DIRECTORY ${grpc_INCLUDE_DIR})
  add_custom_target(grpc_cpp_plugin DEPENDS grpc_grpc protoc)
else()
  set(grpc_INCLUDE_DIR ${GRPC_INCLUDE_DIR})
  set(grpc_LIBRARY ${GRPC_LIBRARY})
  set(grpc_grpc++_LIBRARY ${GRPC_GRPC++_LIBRARY})
  set(grpc_grpc++_reflection_LIBRARY ${GRPC_GRPC++_REFLECTION_LIBRARY})
  set(grpc_CPP_PLUGIN ${GRPC_CPP_PLUGIN})
  add_custom_target(grpc_cpp_plugin DEPENDS protoc)
endif ()

message(STATUS "${grpc_CPP_PLUGIN}")

# libgrpc
add_library(grpc SHARED IMPORTED)
set_target_properties(grpc PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES ${grpc_INCLUDE_DIR}
  IMPORTED_LOCATION ${grpc_LIBRARY}
  )

# libgrpc++
add_library(grpc++ SHARED IMPORTED)
set_target_properties(grpc++ PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES ${grpc_INCLUDE_DIR}
  IMPORTED_LOCATION ${grpc_grpc++_LIBRARY}
  )

# libgrpc++_reflection
add_library(grpc++_reflection SHARED IMPORTED)
set_target_properties(grpc++_reflection PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES ${grpc_INCLUDE_DIR}
  IMPORTED_LOCATION ${grpc_grpc++_reflection_LIBRARY}
  )

if (NOT GRPC_FOUND)
  add_dependencies(grpc grpc_grpc)
  add_dependencies(grpc++ grpc_grpc)
  add_dependencies(grpc++_reflection grpc_grpc)
endif ()

##########################
#        optional        #
##########################
ExternalProject_Add(akrzemi1_Optional
  GIT_REPOSITORY "https://github.com/akrzemi1/Optional.git"
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
  INSTALL_COMMAND "" # remove install step
  UPDATE_COMMAND "" # remove update step
  TEST_COMMAND "" # remove test step
  )
ExternalProject_Get_Property(akrzemi1_Optional source_dir)
set(optional_INCLUDE_DIR ${source_dir})
file(MAKE_DIRECTORY ${optional_INCLUDE_DIR})

add_library(optional INTERFACE IMPORTED)
set_target_properties(optional PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES ${optional_INCLUDE_DIR}
  )

add_dependencies(optional akrzemi1_Optional)

#############################
#         speedlog          #
#############################
ExternalProject_Add(gabime_spdlog
  GIT_REPOSITORY    "https://github.com/gabime/spdlog.git"
  GIT_TAG           "v0.13.0"
  CONFIGURE_COMMAND "" # remove configure step
  BUILD_COMMAND     "" # remove build step
  INSTALL_COMMAND   "" # remove install step
  TEST_COMMAND      "" # remove test step
  UPDATE_COMMAND    "" # remove update step
  )
ExternalProject_Get_Property(gabime_spdlog source_dir)
set(spdlog_INCLUDE_DIRS ${source_dir}/include)
file(MAKE_DIRECTORY ${spdlog_INCLUDE_DIRS})

add_library(spdlog INTERFACE IMPORTED)
set_target_properties(spdlog PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES ${spdlog_INCLUDE_DIRS}
  )

add_dependencies(spdlog gabime_spdlog)
