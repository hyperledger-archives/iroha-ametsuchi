find_path(Optional_INCLUDE_DIR optional.hpp)
mark_as_advanced(Optional_INCLUDE_DIR)

find_package(PackageHandleStandardArgs REQUIRED)
find_package_handle_standard_args(Optional
  REQUIRED_VARS Optional_INCLUDE_DIR
  )

if (Optional_FOUND)
  add_library(optional INTERFACE IMPORTED)
  set_target_properties(optional PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES ${Optional_INCLUDE_DIR}
    )
endif ()
