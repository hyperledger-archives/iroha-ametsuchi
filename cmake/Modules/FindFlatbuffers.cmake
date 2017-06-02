find_path(Flatbuffers_INCLUDE_DIRS flatbuffers/flatbuffers.h)

if(Flatbuffers_INCLUDE_DIRS)
  file(STRINGS "${Flatbuffers_INCLUDE_DIRS}/flatbuffers/flatbuffers.h" Flatbuffers_version_str REGEX "#define FLATBUFFERS_VERSION_[A-Z]+ ")
  string(REGEX REPLACE ".*#define FLATBUFFERS_VERSION_MAJOR ([0-9]+).*" "\\1" Flatbuffers_MAJOR_VERSION "${Flatbuffers_version_str}")
  string(REGEX REPLACE ".*#define FLATBUFFERS_VERSION_MINOR ([0-9]+).*" "\\1" Flatbuffers_MINOR_VERSION "${Flatbuffers_version_str}")
  string(REGEX REPLACE ".*#define FLATBUFFERS_VERSION_REVISION ([0-9]+).*" "\\1" Flatbuffers_REVISION_VERSION "${Flatbuffers_version_str}")
  set(flatbuffers_VERSION "${Flatbuffers_MAJOR_VERSION}.${Flatbuffers_MINOR_VERSION}.${Flatbuffers_REVISION_VERSION}")
endif()

find_library(Flatbuffers_LIBRARIES flatbuffers)

find_program(Flatbuffers_flatc_EXECUTABLE flatc)
if(Flatbuffers_flatc_EXECUTABLE)
  set(Flatbuffers_flatc_FOUND TRUE)
endif()

find_package(PackageHandleStandardArgs REQUIRED)
find_package_handle_standard_args(Flatbuffers
  REQUIRED_VARS Flatbuffers_INCLUDE_DIRS Flatbuffers_LIBRARIES Flatbuffers_flatc_EXECUTABLE
  VERSION_VAR Flatbuffers_VERSION
  HANDLE_COMPONENTS
  )

