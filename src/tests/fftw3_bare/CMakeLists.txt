cmake_minimum_required(VERSION 3.9)

project(fftw3_test VERSION 0.1 LANGUAGES C CXX)

#--- set a default build type if none was specified ---------------------------
set(default_build_type "Debug")

#--- the next two lines are needed that the math functions are found ----------
set(CMAKE_REQUIRED_INCLUDES math.h)
set(CMAKE_REQUIRED_LIBRARIES m) 

include(CheckTypeSize)
include(CheckIncludeFiles)
include(CheckFunctionExists)
check_include_files(alloca.h HAVE_ALLOCA_H)
check_include_files("sys/ipc.h;sys/shm.h" HAVE_SHMGET)
check_include_files(dlfcn.h HAVE_DLFCN_H)
check_function_exists(erf HAVE_ERF)
check_function_exists(getloadavg HAVE_GETLOADAVG)
check_include_files(inttypes.h HAVE_INTTYPES_H)
check_include_files(memory.h HAVE_MEMORY_H)
check_function_exists(powl HAVE_POWL)
check_include_files(memory.h HAVE_MEMORY_H)
check_include_files(stdint.h HAVE_STDINT_H)
check_include_files(stdlib.h HAVE_STDLIB_H)
check_include_files(string.h HAVE_STRING_H)
check_include_files(strings.h HAVE_STRINGS_H)
check_include_files(sys/stat.h HAVE_SYS_STAT_H)
check_include_files(sys/types.h HAVE_SYS_TYPES_H)
check_include_files(sys/unistd.h HAVE_UNISTD_H)
check_type_size("long double" LONG_DOUBLE)
check_type_size("double" DOUBLE)
if (${LONG_DOUBLE} GREATER ${DOUBLE})
  set(HAVE_LONG_DOUBLE 1)
  set(HAVE_LONG_DOUBLE_WIDER 1)
endif (${LONG_DOUBLE} GREATER ${DOUBLE})

#--- add path to my own find modules and other stuff
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../../cmake)

#--- check for fftw3 ----------------------------------------------------------
find_package(FFTW3 REQUIRED)
message(" FFTW3_INCLUDE_DIR: ${FFTW3_INCLUDE_DIR}")
message(" FFTW3_LIBRARY    : ${FFTW3_LIBRARY}")

add_executable(fftw3_test fftw3_test.cpp)
target_include_directories(fftw3_test
  BEFORE PRIVATE
    $<BUILD_INTERFACE:${FFTW3_INCLUDE_DIR}>
)
target_link_libraries(fftw3_test ${FFTW3_LIBRARY})
