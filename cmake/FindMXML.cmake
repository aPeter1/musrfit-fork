# - find MXML
# find the MXML lib and includes
# This module defines 
#  LIBMXML_INCLUDE_DIR, where to find mxml.h
#  LIBMXML_LIBRARY, library to link against
#  LIBMXML_FOUND, if false, do not try to use the MXML lib

find_path(LIBMXML_INCLUDE_DIR mxml.h
  HINT "/usr/include"
)
# find position of mxml.h from the end
string(FIND "${LIBMXML_INCLUDE_DIR}" "/mxml.h" pos REVERSE)
# truncate the string
string(SUBSTRING "${LIBMXML_INCLUDE_DIR}" 0 ${pos} substr)
set(LIBMXML_INCLUDE_DIR ${substr})
unset(substr)

find_library(LIBMXML_LIBRARY mxml)

# get version string
# currently do not know from where to get it automatically

# handle the QUIETLY and REQUIRED arguments and set LIBMXML_FOUND to TRUE if
# all listed variables are TRUE
include(${CMAKE_ROOT}/Modules/FindPackageHandleStandardArgs.cmake)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(MXML
                                  REQUIRED_VARS LIBMXML_LIBRARY LIBMXML_INCLUDE_DIR)

if (NOT LIBMXML_FOUND)
  unset(LIBMXML_LIBRARY)
endif()

mark_as_advanced(LIBMXML_INCLUDE_DIR LIBMXML_LIBRARY)

