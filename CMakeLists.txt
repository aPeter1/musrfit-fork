# - musrfit 
cmake_minimum_required(VERSION 3.6)

project(musrfit VERSION 1.3.0 LANGUAGES C CXX)

#--- musrfit specific options -------------------------------------------------
option(nexus "build optional NeXus support. Needed for ISIS" OFF)
option(ASlibs "build optional ASlibs" OFF)
option(BMWlibs "build optional BMWlibs" OFF)
option(BNMRlibs "build optional beta-NMR libs" OFF)
option(qt_based_tools "try to install Qt based tools (musredit, musrWiz, musrStep, mupp)" ON)
option(try_OpenMP "try to use OpenMP if available" ON)
# define qt_version with possible values 'auto' or version '3', '4', '5'
set(qt_version AUTO CACHE STRING "provide a specific Qt version to be used.")
set_property(CACHE qt_version PROPERTY STRINGS AUTO 3 4 5)

#--- set a default build type if none was specified ---------------------------
set(default_build_type "Release")
 
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
check_function_exists(fork HAVE_FORK)
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

#--- add path to my own find modules and other stuff
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

#--- check for git ------------------------------------------------------------
find_package(Git REQUIRED)

#--- check for ROOT -----------------------------------------------------------
find_package(ROOT 6.06 REQUIRED COMPONENTS Gui MathMore Minuit2 XMLParser)
if (ROOT_mathmore_FOUND)
  execute_process(COMMAND root-config --bindir OUTPUT_VARIABLE ROOT_BINDIR)
  string(STRIP ${ROOT_BINDIR} ROOT_BINDIR)
  execute_process(COMMAND root-config --version OUTPUT_VARIABLE ROOT_VERSION)
  string(STRIP ${ROOT_VERSION} ROOT_VERSION)
  message("-- Found ROOT: ${ROOT_BINDIR} (found version: ${ROOT_VERSION})")
  #---Define useful ROOT functions and macros (e.g. ROOT_GENERATE_DICTIONARY)
  include(${ROOT_USE_FILE})
endif (ROOT_mathmore_FOUND)

#--- check for boost ----------------------------------------------------------
find_package(Boost REQUIRED)

#--- check for gsl ------------------------------------------------------------
find_package(GSL REQUIRED)

#--- check for fftw3 ----------------------------------------------------------
find_package(FFTW3 REQUIRED)
if (FFTW3_THREAD_FOUND)
  set(HAVE_LIBFFTW3_THREADS 1 CACHE INTERNAL "fftw3_threads are available")
endif (FFTW3_THREAD_FOUND)
if (FFTW3F_THREAD_FOUND)
  set(HAVE_LIBFFTW3F_THREADS 1 CACHE INTERNAL "fftw3f_threads are available")
endif (FFTW3F_THREAD_FOUND)

#--- check for libxml2 --------------------------------------------------------
find_package(LibXml2 REQUIRED)

#--- check for OpenMP ---------------------------------------------------------
if (try_OpenMP AND NOT APPLE)
  find_package(OpenMP)
  if (OpenMP_FOUND)
    add_definitions(-DHAVE_GOMP)
    set(HAVE_GOMP 1 CACHE INTERNAL "Have GOMP")
  endif (OpenMP_FOUND)
endif (try_OpenMP AND NOT APPLE)

#--- check for Qt -------------------------------------------------------------
if (qt_based_tools)
  # check for any Qt, i.e. AUTO
  if (qt_version STREQUAL AUTO)
    # first try Qt5
    # Find the QtCore library
    find_package(Qt5Core)
    if (Qt5Core_FOUND)
      # Find the QtWidgets library
      find_package(Qt5Widgets CONFIG REQUIRED)
      # Find the QtXml library
      find_package(Qt5Xml CONFIG REQUIRED)
      # Find the QtNetwork library
      find_package(Qt5Network CONFIG REQUIRED)
      # Find the QtSvg library
      find_package(Qt5Svg CONFIG REQUIRED)
      # Fing the QtPrintSupport
      find_package(Qt5PrintSupport CONFIG REQUIRED)
    endif (Qt5Core_FOUND)

    # if Qt5 is not found, try Qt4
    if (NOT Qt5Core_FOUND)
      find_package(Qt4 COMPONENTS QtGui QtWebKit QtXml)
    endif (NOT Qt5Core_FOUND)

    # if Qt5 and Qt4 is not found try Qt3. Hopefully you never reach this point
    if (NOT Qt5Core_FOUND)
      if (NOT Qt4_FOUND)
        find_package(Qt3)
      endif (NOT Qt4_FOUND)
    endif (NOT Qt5Core_FOUND)
  endif (qt_version STREQUAL AUTO)

  # check specifically for Qt5
  if (qt_version STREQUAL 5)
    # Find the QtCore library
    find_package(Qt5Core)
    if (Qt5Core_FOUND)
      # Find the QtWidgets library
      find_package(Qt5Widgets CONFIG REQUIRED)
      # Find the QtXml library
      find_package(Qt5Xml CONFIG REQUIRED)
      # Find the QtNetwork library
      find_package(Qt5Network CONFIG REQUIRED)
      # Find the QtSvg library
      find_package(Qt5Svg CONFIG REQUIRED)
      # Fing the QtPrintSupport
      find_package(Qt5PrintSupport CONFIG REQUIRED)
    else (Qt5Core_FOUND)
      message(FATAL_ERROR "Couldn't find the specifically requested Qt5 version.")
    endif (Qt5Core_FOUND)
  endif (qt_version STREQUAL 5)

  # check specifically for Qt4
  if (qt_version STREQUAL 4)
    find_package(Qt4 COMPONENTS QtGui QtWebKit QtXml)
    if (NOT Qt4_FOUND)
      message(FATAL_ERROR "Couldn't find the specifically requested Qt4 version.")
    endif (NOT Qt4_FOUND)
  endif (qt_version STREQUAL 4)

  # check specifically for Qt3
  if (qt_version STREQUAL 3)
    find_package(Qt3)
    if (NOT QT_FOUND)
      message(FATAL_ERROR "Couldn't find the specifically requested Qt3 version.")
    endif (NOT QT_FOUND)
  endif (qt_version STREQUAL 3)
endif (qt_based_tools)

#--- if NeXus check also for HDF4, HDF5, and MXML -----------------------------
if (nexus)
  find_package(HDF5 COMPONENTS CXX REQUIRED )
#//as35 - STILL NEEDED?? -> find_package(HDF4 REQUIRED)
  find_package(MXML REQUIRED)
  find_package(NeXus REQUIRED)
  add_definitions(-DPNEXUS_ENABLED)
endif (nexus)

#--- check for Cuba lib if BMWlibs are enabled --------------------------------
#//as35 probably always stick to the internal one ...

#--- all checks done -> feed config.h -----------------------------------------
set(HAVE_CONFIG_H 1 CACHE INTERNAL "config.h is available")
configure_file(${CMAKE_SOURCE_DIR}/cmake/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)

#--- propagate to the sub-directories -----------------------------------------
add_subdirectory(src)

#--- write summary of the installation 
message("")
message("|-----------------------------------------------------------------------|")
message("|                                                                       |")
message("|                            Summary                                    |")
message("|                                                                       |")
message("|-----------------------------------------------------------------------|")
message("")
message(" System: ${CMAKE_HOST_SYSTEM_NAME} ${CMAKE_SYSTEM_PROCESSOR} - ${CMAKE_HOST_SYSTEM_VERSION}")
message(" -------")
message("")
message(" Build Type: ${CMAKE_BUILD_TYPE}")
message(" -----------")
message("")
message(" Requirements:")
message(" -------------")
message("")
message(" FFTW3   found in ${FFTW3_INCLUDE_DIR}")
message(" GSL     found in ${GSL_INCLUDE_DIRS}")
message(" BOOST   found in ${Boost_INCLUDE_DIRS}")
message(" LibXML2 found in ${LIBXML2_INCLUDE_DIR}")
message(" ROOT    found in ${ROOT_INCLUDE_DIRS}")
if (OpenMP_FOUND)
  if (OpenMP_CXX_VERSION)
    message(" OpenMP  found (version ${OpenMP_CXX_VERSION})")
  else (OpenMP_CXX_VERSION)
    message(" OpenMP  found")
  endif (OpenMP_CXX_VERSION)
endif (OpenMP_FOUND)

if (nexus)
  message("")
#//as35  message(" HDF4   found in ${HDF4_INCLUDE_DIRS}")
  message(" HDF5    found in ${HDF5_INCLUDE_DIRS}")
  message(" NeXus   found in ${NEXUS_INCLUDE_DIR}") 
endif (nexus)

message("")
if (qt_based_tools)
  if (Qt5Core_FOUND)
    message(" Qt      found in  ${Qt5Core_INCLUDE_DIRS} (Version: ${Qt5Core_VERSION})")
  else (Qt5Core_FOUND)
    if (Qt4_FOUND)
      message(" Qt      found (Version: ${QT_VERSION_MAJOR}.${QT_VERSION_MINOR}.${QT_VERSION_PATCH})")
    else (Qt4_FOUND)
      if (QT_FOUND)
        message(" Qt      found (Version: ${QT_VERSION})")
      endif (QT_FOUND)
    endif (Qt4_FOUND)
  endif (Qt5Core_FOUND)
endif (qt_based_tools)
message("")
message(" Features:")
message(" ---------")
message("")
message(" Supported muSR file formates:")
message("   MusrRoot                           : yes")
message("   ROOT (LEM)                         : yes")
message("   MUD (triumf)                       : yes")
message("   PSI-BIN                            : yes")
message("   PSI-MDU                            : yes")
message("   WKM (deprecated)                   : yes")
if (nexus)
  message("   NeXus                              : yes")
else (nexus)
  message("   NeXus                              : no")
endif (nexus)

message("")
message(" External user-function libraries:")
if (ASlibs)
  message("   ASlibs                             : yes")
else (ASlibs)
  message("   ASlibs                             : no")
endif (ASlibs)
if (BMWlibs)
  message("   BMWlibs                            : yes")
else (BMWlibs)
  message("   BMWlibs                            : no")
endif (BMWlibs)
if (BNMRlibs)
  message("   BNMRlibs                           : yes")
else (BNMRlibs)
  message("   BNMRlibs                           : no")
endif (BNMRlibs)

if (qt_based_tools)
if (Qt5Core_FOUND)
  message("")
  message(" Qt5 based tools:")
  message("   musredit, musrStep, musrWiz, mupp  : yes")
endif (Qt5Core_FOUND)
if (Qt4_FOUND)
  message("")
  message(" Qt4 based tools (deprecated):")
  message("   musredit                           : yes")
endif (Qt4_FOUND)
if (QT_FOUND)
  message("")
  message(" Qt3 based tools (outdated):")
  message("   musrgui                            : yes")
endif (QT_FOUND)
if (NOT Qt5Core_FOUND AND NOT Qt4_FOUND AND NOT QT_FOUND)
  message("")
  message(" NO Qt based tools will be installed since Qt is not found or not installed on the system")
endif (NOT Qt5Core_FOUND AND NOT Qt4_FOUND AND NOT QT_FOUND)
else (qt_based_tools)
  message("")
  message(" Qt based tools (musredit, musrStep, musrWiz, mupp) have been disabled")
endif (qt_based_tools)
message("")
message(" Installation directories:")
message(" -------------------------")
message("")
message(" Programs                : ${CMAKE_INSTALL_PREFIX}/bin")
message(" XML configuration files : " $ENV{HOME} "/.musrfit")
message(" Documentation           : ${CMAKE_INSTALL_PREFIX}/share/doc/musrfit")
message("")
message("-------------------------------------------------------------------------")
message("")

#--- end ----------------------------------------------------------------------
