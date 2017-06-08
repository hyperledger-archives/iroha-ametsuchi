find_path(SPDlog_INCLUDE_DIR spdlog/spdlog.h)
mark_as_advanced(SPDlog_INCLUDE_DIR)

find_package(PackageHandleStandardArgs REQUIRED)
find_package_handle_standard_args(SPDlog
  REQUIRED_VARS SPDlog_INCLUDE_DIR
  )

if (SPDlog_FOUND)
  add_library(spdlog INTERFACE IMPORTED)
  set_target_properties(spdlog PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES ${SPDlog_INCLUDE_DIR}
    )
endif ()
