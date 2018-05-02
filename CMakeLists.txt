# - musrfit 
cmake_minimum_required(VERSION 3.6)

project(musrfit VERSION 1.3.0 LANGUAGES CXX)

#--- musrfit specific options -------------------------------------------------------------------
option(nexus "build optional NeXus support. Needed for ISIS" OFF)
option(ASlibs "build optional ASlibs" OFF)
option(BMWlibs "build optional BMWlibs" OFF)
option(BNMRlibs "build optional beta-NMR libs" OFF)
option(qt_based_tools "try to install Qt based tools" ON)

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
if (qt_based_tools)
  # first try Qt5
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

  # if Qt5 is not found, try Qt4
  if (NOT Qt5Core_FOUND)
    find_package(Qt4 COMPONENTS QtGui QtWebKit QtXml)
    message("Qt4 found: ${Qt4_FOUND} //as35")
  endif (NOT Qt5Core_FOUND)

  # if Qt5 and Qt4 is not found try Qt3. Hopefully you never reach this point
  if (NOT Qt4_FOUND AND NOT Qt5_FOUND)
    find_package(Qt3)
    message("Qt3 found: ${QT_FOUND} //as35")
  endif (NOT Qt4_FOUND AND NOT Qt5_FOUND)
endif (qt_based_tools)

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

#--- write summary of the installation 
message("")
message("|-----------------------------------------------------------------------|")
message("|                                                                       |")
message("|                            Summary                                    |")
message("|                                                                       |")
message("|-----------------------------------------------------------------------|")
message("")
message(" System:")
message(" -------")
message("") 
message(" ${CMAKE_HOST_SYSTEM_NAME} ${CMAKE_SYSTEM_PROCESSOR} - ${CMAKE_HOST_SYSTEM_VERSION}")
message("")
message(" Requirements:")
message(" -------------")
message("")
message(" FFTW3   found in //as35 STILL MISSING")
message(" GSL     found in ${GSL_INCLUDE_DIRS}")
message(" BOOST   found in ${Boost_INCLUDE_DIRS}")
message(" LibXML2 found in ${LIBXML2_INCLUDE_DIR}")
message(" ROOT    found in ${ROOT_INCLUDE_DIRS}")

if (nexus)
  message("")
  message(" HDF4   found in ${HDF4_INCLUDE_DIRS}")
  message(" HDF5   found in ${HDF5_INCLUDE_DIRS}")
  message(" NeXus  found in //as35 STILL MISSING") 
endif (nexus)

if (BMWlibs)
  message("")
  message(" CUBA   found in //as35 STILL MISSING")
endif (BMWlibs)

message("")
message(" Qt     found in  ${Qt5Core_INCLUDE_DIRS} (Version: ${Qt5Core_VERSION})")
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

#--- end ---
