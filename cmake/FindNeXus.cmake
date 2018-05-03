# - Find NeXus library
# Find the native NEXUS includes and library
# This module defines
#  NEXUS_INCLUDE_DIR, where to find NeXus.h, etc.
#  NEXUS_LIBRARY, library to link against to use NEXUS
#  NEXUS_FOUND, if false, do not try to use NEXUS.

find_path(NEXUS_INCLUDE_DIR napi.h
  HINTS "/usr/local/include" "/opt/nexus/include" "/usr/local/include/nexus"
)
# find position of napi.h from the end
string(FIND "${NEXUS_INCLUDE_DIR}" "/napi.h" pos REVERSE)
# truncate the string
string(SUBSTRING "${NEXUS_INCLUDE_DIR}" 0 ${pos} substr)
set(NEXUS_INCLUDE_DIR ${substr})
unset(substr)

find_library(NEXUS_LIBRARY NeXus 
  HINTS "/usr/lib" "/usr/lib64" "/usr/local/lib" "/usr/local/lib64" "/opt/nexus/lib")

# get version string
if (NEXUS_INCLUDE_DIR AND EXISTS ${NEXUS_INCLUDE_DIR}/napi.h)
  file(STRINGS "${NEXUS_INCLUDE_DIR}/napi.h" NEXUS_version_str
       REGEX "^#define[\t ]+NEXUS_VERSION[\t ].*")

  string(REGEX REPLACE "^#define[\t ]+NEXUS_VERSION[\t ]+\"([^\"]*).*"
         "\\1" NEXUS_VERSION_STRING "${NEXUS_version_str}")
  unset(NEXUS_version_str)
endif()

# handle the QUIETLY and REQUIRED arguments and set NEXUS_FOUND to TRUE if
# all listed variables are TRUE
include(${CMAKE_ROOT}/Modules/FindPackageHandleStandardArgs.cmake)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(NEXUS
                                  REQUIRED_VARS NEXUS_LIBRARY NEXUS_INCLUDE_DIR
                                  VERSION_VAR NEXUS_VERSION_STRING)

if (NOT NEXUS_FOUND)
  unset(NEXUS_LIBRARY)
endif()

mark_as_advanced(NEXUS_INCLUDE_DIR NEXUS_LIBRARY)



