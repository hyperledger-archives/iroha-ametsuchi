find_path(LMDB_INCLUDE_DIRS lmdb.h)

find_library(LMDB_LIBRARIES lmdb)

find_package(PackageHandleStandardArgs REQUIRED)
find_package_handle_standard_args(LMDB
  REQUIRED_VARS LMDB_INCLUDE_DIRS LMDB_LIBRARIES
  )
