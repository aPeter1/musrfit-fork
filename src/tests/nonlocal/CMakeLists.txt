# main cmake file for nonlocal
#
# $Id$
# author: Andreas Suter, andreas.suter@psi.ch

cmake_minimum_required (VERSION 2.8)
project (nonlocal)

# set version number
set (nonlocal_VERSION_MAJOR 0)
set (nonlocal_VERSION_MINOR 1)

# configure a header file to pass some of the CMake settings
# to the source code
configure_file (
  "${PROJECT_SOURCE_DIR}/nonlocalConfig.h.in"
  "${PROJECT_BINARY_DIR}/nonlocalConfig.h"
  )
 
# add the binary tree to the search path for include files
# so that we will find TutorialConfig.h
include_directories("${PROJECT_BINARY_DIR}")

# CERN/ROOT related stuff ----------------------------------------------
# Load some basic macros which are needed later on
include(FindROOT.cmake)

# includes for ROOT
include_directories(${ROOT_INCLUDE_DIR})
link_directories(${ROOT_LIBRARY_DIR})
message("-- CERN/ROOT include directory " ${ROOT_INCLUDE_DIR})
message("-- CERN/ROOT library directory " ${ROOT_LIBRARY_DIR})
message("-- CERN/ROOT libraries: " ${ROOT_LIBRARIES})

# GSL related stuff ----------------------------------------------------
# includes for GSL
set(GSL_INCLUDE_DIR /usr/include/gsl)
set(GSL_LIBRARY_DIR /usr/lib64)
set(GSL_LIBS -lgsl\ -lgslcblas)
include_directories(${GSL_INCLUDE_DIR})
link_directories(${GSL_LIBRARY_DIR})
message("-- GSL include directory " ${GSL_INCLUDE_DIR})
message("-- GSL library directory " ${GSL_LIBRARY_DIR})
message("-- GSL libraries: " ${GSL_LIBS})

# Eigen related stuff --------------------------------------------------
# includes for Eigen
set(EIGEN_INCLUDE_DIR /opt/eigen)
include_directories(${EIGEN_INCLUDE_DIR})
message("-- Eigen include directory " ${EIGEN_INCLUDE_DIR})

# FFTW3 related stuff ---------------------------------------------------
# includes for FFTW
set(FFTW3_INCLUDE_DIR /usr/include)
set(FFTW3_LIBRARY_DIR /usr/lib64)
set(FFTW3_LIBS -lfftw3)
include_directories(${FFTW3_INCLUDE_DIR})
link_directories(${FFTW3_LIBRARY_DIR})
message("-- FFTW3 include directory " ${FFTW3_INCLUDE_DIR})
message("-- FFTW3 library directory " ${FFTW3_LIBRARY_DIR})
message("-- FFTW3 library " ${FFTW3_LIBS})

add_library(pippard PPippard.cpp PPippard.h)

add_executable(nonlocal nonlocal.cpp)

target_link_libraries(nonlocal pippard ${GSL_LIBS} ${ROOT_LIBRARIES} ${FFTW3_LIBS})

# install stuff ------------------------------------------------------
set(CMAKE_INSTALL_PREFIX $ENV{HOME})
message("-- will install nonlocal in " ${CMAKE_INSTALL_PREFIX}/bin)
install_targets(/bin nonlocal)

# uninstall target ---------------------------------------------------
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
