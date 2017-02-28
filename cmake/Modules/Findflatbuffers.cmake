find_path(flatbuffers_INCLUDE_DIRS flatbuffers/flatbuffers.h)

if(flatbuffers_INCLUDE_DIRS)
  file(STRINGS "${flatbuffers_INCLUDE_DIRS}/flatbuffers/flatbuffers.h" flatbuffers_version_str REGEX "#define FLATBUFFERS_VERSION_[A-Z]+ ")
  string(REGEX REPLACE ".*#define FLATBUFFERS_VERSION_MAJOR ([0-9]+).*" "\\1" flatbuffers_MAJOR_VERSION "${flatbuffers_version_str}")
  string(REGEX REPLACE ".*#define FLATBUFFERS_VERSION_MINOR ([0-9]+).*" "\\1" flatbuffers_MINOR_VERSION "${flatbuffers_version_str}")
  string(REGEX REPLACE ".*#define FLATBUFFERS_VERSION_REVISION ([0-9]+).*" "\\1" flatbuffers_REVISION_VERSION "${flatbuffers_version_str}")
  set(flatbuffers_VERSION "${flatbuffers_MAJOR_VERSION}.${flatbuffers_MINOR_VERSION}.${flatbuffers_REVISION_VERSION}")
endif()

find_library(flatbuffers_LIBRARIES flatbuffers)

if(flatbuffers_FIND_REQUIRED_flatc)
  find_program(flatc_EXECUTABLE flatc)
  if(flatc_EXECUTABLE)
    set(flatbuffers_flatc_FOUND TRUE)
  endif()
endif()

find_package(PackageHandleStandardArgs REQUIRED)
find_package_handle_standard_args(flatbuffers
  REQUIRED_VARS flatbuffers_INCLUDE_DIRS flatbuffers_LIBRARIES
  VERSION_VAR flatbuffers_VERSION
  HANDLE_COMPONENTS
  )

