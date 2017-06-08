find_path(CPPfs_INCLUDE_DIR cppfs/cppfs.h)
mark_as_advanced(CPPfs_INCLUDE_DIR)

find_library(CPPfs_LIBRARY cppfs)
mark_as_advanced(CPPfs_LIBRARY)

find_package(PackageHandleStandardArgs REQUIRED)
find_package_handle_standard_args(CPPfs
  REQUIRED_VARS CPPfs_INCLUDE_DIR CPPfs_LIBRARY
  )

if (CPPfs_FOUND)
  add_library(cppfs UNKNOWN IMPORTED)
  set_target_properties(cppfs PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES ${CPPfs_INCLUDE_DIR}
    INTERFACE_LINK_LIBRARIES "cppexpose;cppassist;cpplocate;ssh2;ssl;crypto"
    IMPORTED_LOCATION ${CPPfs_LIBRARY}
    )
endif ()
