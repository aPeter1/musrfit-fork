# - musrfit 
cmake_minimum_required(VERSION 3.9)

if (CMAKE_VERSION GREATER_EQUAL 3.12)
  cmake_policy(SET CMP0075 NEW)
endif (CMAKE_VERSION GREATER_EQUAL 3.12)

project(vortex-film VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 11)
#set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#--- set a default build type if none was specified ---------------------------
set(default_build_type "Debug")
 
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
  set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE
      STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
    "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

#--- perform some checks and generate the config.h ----------------------------

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

#--- check for all the needed packages ----------------------------------------

#--- check for git ------------------------------------------------------------
find_package(Git REQUIRED)

#--- check for ROOT -----------------------------------------------------------
find_package(ROOT 6.18 REQUIRED COMPONENTS Gui MathMore Minuit2 XMLParser)
if (ROOT_mathmore_FOUND)
  execute_process(COMMAND root-config --bindir OUTPUT_VARIABLE ROOT_BINDIR)
  string(STRIP ${ROOT_BINDIR} ROOT_BINDIR)
  execute_process(COMMAND root-config --version OUTPUT_VARIABLE ROOT_VERSION)
  string(STRIP ${ROOT_VERSION} ROOT_VERSION)
  message("-- Found ROOT: ${ROOT_BINDIR} (found version: ${ROOT_VERSION})")
  #---Define useful ROOT functions and macros (e.g. ROOT_GENERATE_DICTIONARY)
  include(${ROOT_USE_FILE})
endif (ROOT_mathmore_FOUND)

#--- define the BMWlibs -------------------------------------------------------
set(BMWlibs
  BMWtools
  FitPofB
)

#--- add all executables ------------------------------------------------------
add_executable(vortex_film 
  vortex_film.cpp
)
target_include_directories(vortex_film
  BEFORE PRIVATE
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}>
)
target_link_directories(vortex_film
  BEFORE PRIVATE
    $ENV{ROOTSYS}/lib
)
target_link_libraries(vortex_film ${ROOT_LIBRARIES} ${BMWlibs})

#--- end ----------------------------------------------------------------------
