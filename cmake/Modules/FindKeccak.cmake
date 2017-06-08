find_path(Keccak_INCLUDE_DIR KeccakCodePackage.h)
mark_as_advanced(Keccak_INCLUDE_DIR)

find_library(Keccak_LIBRARY keccak)
mark_as_advanced(Keccak_LIBRARY)

find_package(PackageHandleStandardArgs REQUIRED)
find_package_handle_standard_args(Keccak
  REQUIRED_VARS Keccak_INCLUDE_DIR Keccak_LIBRARY
  )

if (Keccak_FOUND)
  add_library(keccak UNKNOWN IMPORTED)
  set_target_properties(keccak PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES ${Keccak_INCLUDE_DIR}
    IMPORTED_LOCATION ${Keccak_LIBRARY}
    IMPORTED_LINK_INTERFACE_LANGUAGES "C"
    )
endif ()
