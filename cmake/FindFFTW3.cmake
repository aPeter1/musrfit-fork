# - Find fftw3 library
# Find the native FFTW3 includes and library
# This module defines
#  FFTW3_INCLUDE_DIR, where to find fftw3.h, etc.
# ---
#  FFTW3_LIBRARY, library to link against to use FFTW3
#  FFTW3_OMP_LIBRARY, library to link against to use FFTW3_omp
#  FFTW3_THREADS_LIBRARY, library to link against to use FFTW3_threads
#  FFTW3_FOUND, if false, do not try to use FFTW3.
# ---
#  FFTW3L_LIBRARY, library to link against to use FFTW3l
#  FFTW3L_OMP_LIBRARY, library to link against to use FFTW3l_omp
#  FFTW3L_THREADS_LIBRARY, library to link against to use FFTW3l_threads
#  FFTW3L_FOUND, if false, do not try to use FFTW3l.
# ---
#  FFTW3F_LIBRARY, library to link against to use FFTW3f
#  FFTW3F_OMP_LIBRARY, library to link against to use FFTW3f_omp
#  FFTW3F_THREADS_LIBRARY, library to link against to use FFTW3f_threads
#  FFTW3F_FOUND, if false, do not try to use FFTW3f.
# ---
#  FFTW3Q_LIBRARY, library to link against to use FFTW3q
#  FFTW3Q_OMP_LIBRARY, library to link against to use FFTW3q_omp
#  FFTW3Q_THREADS_LIBRARY, library to link against to use FFTW3q_threads
#  FFTW3Q_FOUND, if false, do not try to use FFTW3q.

find_path(FFTW3_INCLUDE_DIR fftw3.h
  HINTS "/usr/include" "/opt/local/include"
)

# find position of fftw3.h from the end
string(FIND "${FFTW3_INCLUDE_DIR}" "/fftw3.h" pos REVERSE)
# truncate the string
string(SUBSTRING "${FFTW3_INCLUDE_DIR}" 0 ${pos} substr)
set(FFTW3_INCLUDE_DIR ${substr})
unset(substr)

find_library(FFTW3_LIBRARY fftw3)
find_library(FFTW3_OMP_LIBRARY fftw3_omp)
find_library(FFTW3_THREAD_LIBRARY fftw3_threads)
find_library(FFTW3L_LIBRARY fftw3l)
find_library(FFTW3L_OMP_LIBRARY fftw3l_omp)
find_library(FFTW3L_THREAD_LIBRARY fftw3l_threads)
find_library(FFTW3F_LIBRARY fftw3f)
find_library(FFTW3F_OMP_LIBRARY fftw3f_omp)
find_library(FFTW3F_THREAD_LIBRARY fftw3f_threads)
find_library(FFTW3Q_LIBRARY fftw3q)
find_library(FFTW3Q_OMP_LIBRARY fftw3q_omp)
find_library(FFTW3Q_THREAD_LIBRARY fftw3q_threads)

# handle the QUIETLY and REQUIRED arguments and set FFTW3_FOUND to TRUE if
# all listed variables are TRUE
include(${CMAKE_ROOT}/Modules/FindPackageHandleStandardArgs.cmake)
find_package_handle_standard_args(FFTW3
                                  REQUIRED_VARS FFTW3_LIBRARY FFTW3_INCLUDE_DIR
                                  VERSION_VAR "3")
find_package_handle_standard_args(FFTW3L
                                  REQUIRED_VARS FFTW3L_LIBRARY FFTW3_INCLUDE_DIR
                                  VERSION_VAR "3")
find_package_handle_standard_args(FFTW3F
                                  REQUIRED_VARS FFTW3F_LIBRARY FFTW3_INCLUDE_DIR
                                  VERSION_VAR "3")
find_package_handle_standard_args(FFTW3Q
                                  REQUIRED_VARS FFTW3Q_LIBRARY FFTW3_INCLUDE_DIR
                                  VERSION_VAR "3")

if (NOT FFTW3_FOUND)
  unset(FFTW3_LIBRARY)
endif()
if (NOT FFTW3L_FOUND)
  unset(FFTW3L_LIBRARY)
endif()
if (NOT FFTW3F_FOUND)
  unset(FFTW3F_LIBRARY)
endif()
if (NOT FFTW3Q_FOUND)
  unset(FFTW3Q_LIBRARY)
endif()

mark_as_advanced(
  FFTW3_INCLUDE_DIR 
  FFTW3_LIBRARY FFTW3_OMP_LIBRARY FFTW3_THREAD_LIBRARY
  FFTW3L_LIBRARY FFTW3L_OMP_LIBRARY FFTW3L_THREAD_LIBRARY
  FFTW3F_LIBRARY FFTW3F_OMP_LIBRARY FFTW3F_THREAD_LIBRARY
  FFTW3Q_LIBRARY FFTW3Q_OMP_LIBRARY FFTW3Q_THREAD_LIBRARY
)

if (FFTW3_FOUND)
  message("-- Found FFTW3: ${FFTW3_INCLUDE_DIR}")
endif (FFTW3_FOUND)
