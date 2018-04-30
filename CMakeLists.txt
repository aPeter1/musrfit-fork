# - musrfit 
cmake_minimum_required(VERSION 3.6)

project(musrfit VERSION 1.3.0 LANGUAGES CXX)

#--- musrfit specific options -------------------------------------------------------------------
option(nexus "build optional NeXus support. Needed for ISIS" OFF)
option(ASlibs "build optional ASlibs" OFF)
option(BMWlibs "build optional BMWlibs" OFF)

#--- check for all the needed packages ----------------------------------------------------------

#--- check for git ------------------------------------------------------------------------------
find_package(Git REQUIRED)

#--- check for ROOT -----------------------------------------------------------------------------
find_package(ROOT 6.06 REQUIRED COMPONENTS MathMore Minuit2 XMLParser)
if (ROOT_mathmore_FOUND)
  execute_process(COMMAND root-config --bindir OUTPUT_VARIABLE ROOT_BINDIR)
  string(STRIP ${ROOT_BINDIR} ROOT_BINDIR)
  execute_process(COMMAND root-config --version OUTPUT_VARIABLE ROOT_VERSION)
  string(STRIP ${ROOT_VERSION} ROOT_VERSION)
  message("-- Found ROOT: ${ROOT_BINDIR} (found version: ${ROOT_VERSION})")
endif (ROOT_mathmore_FOUND)

#--- check for boost ----------------------------------------------------------------------------
find_package(Boost REQUIRED)

#--- check for gsl ------------------------------------------------------------------------------
find_package(GSL REQUIRED)

#--- check for fftw3 ----------------------------------------------------------------------------
#[==[ // needs to be written by as35
find_package(FFTW3 REQUIRED)
#]==]

#--- check for libxml2 --------------------------------------------------------------------------
find_package(LibXml2 REQUIRED)

#--- check for OpenMP ---------------------------------------------------------------------------
find_package(OpenMP REQUIRED)

#--- check for Qt -------------------------------------------------------------------------------
# Find the QtCore library
find_package(Qt5Core)
# Find the QtWidgets library
find_package(Qt5Widgets)
# Find the QtXml library
find_package(Qt5Xml)

# only temporarily needed for debug //as35
if (Qt5Core_FOUND)
  message("-- Found Qt5Core")
endif (Qt5Core_FOUND)
if (Qt5Widgets_FOUND)
  message("-- Found Qt5Widgets")
endif (Qt5Widgets_FOUND)
if (Qt5Xml_FOUND)
  message("-- Found Qt5Xml")
endif (Qt5Xml_FOUND)

#--- if NeXus check also for HDF4, HDF5, and MXML -----------------------------------------------
if (nexus)
  find_package(HDF5 REQUIRED)
  find_package(HDF4 REQUIRED)
  find_package(MXML REQUIRED)
  find_package(NeXus REQUIRED)
endif (nexus)

#--- check for Cuba lib if BMWlibs are enabled --------------------------------------------------

#[==[
add_subdirectory(src)
#]==]

#--- end ---
