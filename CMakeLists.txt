# - musrfit 
cmake_minimum_required(VERSION 3.6)

project(musrfit VERSION 1.3.0 LANGUAGES CXX)

#--- musrfit specific options -------------------------------------------------------------------
option(nexus "build optional NeXus support. Needed for ISIS" OFF)
option(ASlibs "build optional ASlibs" OFF)
option(BMWlibs "build optional BMWlibs" OFF)
option(BNMRlibs "build optional beta-NMR libs" OFF)

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

message("")
message(" Qt5 base tools:")
if (Qt5Core_FOUND)
  message("   musredit, musrStep, musrWiz, mupp   : yes")
else (Qt5Core_FOUND)
  message("   musredit, musrStep, musrWiz, mupp   : no")
endif (Qt5Core_FOUND)
message("")
message(" Qt4 base tools (deprecated):")
if (Qt4Core_FOUND)
  message("   musredit                            : yes")
else (Qt4Core_FOUND)
  message("   musredit                            : no")
endif (Qt4Core_FOUND)
message("")
message(" Qt3 base tools (outdated):")
if (Qt3Core_FOUND)
  message("   musredit                            : yes")
else (Qt3Core_FOUND)
  message("   musredit                            : no")
endif (Qt3Core_FOUND)
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
