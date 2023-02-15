# Install script for directory: /Users/maria/Applications/musrfit/src/classes

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/objdump")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/Users/maria/Applications/musrfit/build/src/classes/libPUserFcnBase.1.3.0.dylib")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libPUserFcnBase.1.3.0.dylib" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libPUserFcnBase.1.3.0.dylib")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/opt/homebrew/Cellar/root/6.26.06_2/lib/root"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libPUserFcnBase.1.3.0.dylib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" -x "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libPUserFcnBase.1.3.0.dylib")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/Users/maria/Applications/musrfit/build/src/classes/libPUserFcnBase.dylib")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libPUserFcnBase.dylib" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libPUserFcnBase.dylib")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/opt/homebrew/Cellar/root/6.26.06_2/lib/root"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libPUserFcnBase.dylib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" -x "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libPUserFcnBase.dylib")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/Users/maria/Applications/musrfit/build/src/classes/libPRgeHandler.1.3.0.dylib")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libPRgeHandler.1.3.0.dylib" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libPRgeHandler.1.3.0.dylib")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/opt/homebrew/Cellar/root/6.26.06_2/lib/root"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libPRgeHandler.1.3.0.dylib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" -x "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libPRgeHandler.1.3.0.dylib")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/Users/maria/Applications/musrfit/build/src/classes/libPRgeHandler.dylib")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libPRgeHandler.dylib" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libPRgeHandler.dylib")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/opt/homebrew/Cellar/root/6.26.06_2/lib/root"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libPRgeHandler.dylib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" -x "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libPRgeHandler.dylib")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/Users/maria/Applications/musrfit/build/src/classes/libPMusr.1.3.0.dylib")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libPMusr.1.3.0.dylib" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libPMusr.1.3.0.dylib")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/opt/homebrew/Cellar/root/6.26.06_2/lib/root"
      -delete_rpath "/Users/maria/Applications/musrfit/build/src/classes"
      -delete_rpath "/Users/maria/Applications/musrfit/build/src/external/mud/src"
      -delete_rpath "/Users/maria/Applications/musrfit/build/src/external/MusrRoot"
      -delete_rpath "/Users/maria/Applications/musrfit/build/src/external/TLemRunHeader"
      -delete_rpath "/Users/maria/Applications/musrfit/build/src/external/MuSR_software/Class_MuSR_PSI"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libPMusr.1.3.0.dylib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" -x "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libPMusr.1.3.0.dylib")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/Users/maria/Applications/musrfit/build/src/classes/libPMusr.dylib")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libPMusr.dylib" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libPMusr.dylib")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/opt/homebrew/Cellar/root/6.26.06_2/lib/root"
      -delete_rpath "/Users/maria/Applications/musrfit/build/src/classes"
      -delete_rpath "/Users/maria/Applications/musrfit/build/src/external/mud/src"
      -delete_rpath "/Users/maria/Applications/musrfit/build/src/external/MusrRoot"
      -delete_rpath "/Users/maria/Applications/musrfit/build/src/external/TLemRunHeader"
      -delete_rpath "/Users/maria/Applications/musrfit/build/src/external/MuSR_software/Class_MuSR_PSI"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libPMusr.dylib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" -x "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libPMusr.dylib")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE FILE FILES
    "/Users/maria/Applications/musrfit/build/src/classes/libPFourierCanvas_rdict.pcm"
    "/Users/maria/Applications/musrfit/build/src/classes/libPFourierCanvas.rootmap"
    "/Users/maria/Applications/musrfit/build/src/classes/libPMusrCanvas_rdict.pcm"
    "/Users/maria/Applications/musrfit/build/src/classes/libPMusrCanvas.rootmap"
    "/Users/maria/Applications/musrfit/build/src/classes/libPMusrT0_rdict.pcm"
    "/Users/maria/Applications/musrfit/build/src/classes/libPMusrT0.rootmap"
    "/Users/maria/Applications/musrfit/build/src/classes/libPRgeHandler_rdict.pcm"
    "/Users/maria/Applications/musrfit/build/src/classes/libPRgeHandler.rootmap"
    "/Users/maria/Applications/musrfit/build/src/classes/libPStartupHandler_rdict.pcm"
    "/Users/maria/Applications/musrfit/build/src/classes/libPStartupHandler.rootmap"
    "/Users/maria/Applications/musrfit/build/src/classes/libPUserFcnBase_rdict.pcm"
    "/Users/maria/Applications/musrfit/build/src/classes/libPUserFcnBase.rootmap"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE FILE FILES
    "/Users/maria/Applications/musrfit/src/include/PFitterFcn.h"
    "/Users/maria/Applications/musrfit/src/include/PFitter.h"
    "/Users/maria/Applications/musrfit/src/include/PFourierCanvas.h"
    "/Users/maria/Applications/musrfit/src/include/PFourier.h"
    "/Users/maria/Applications/musrfit/src/include/PFunctionGrammar.h"
    "/Users/maria/Applications/musrfit/src/include/PFunction.h"
    "/Users/maria/Applications/musrfit/src/include/PFunctionHandler.h"
    "/Users/maria/Applications/musrfit/src/include/PMsr2Data.h"
    "/Users/maria/Applications/musrfit/src/include/PMsrHandler.h"
    "/Users/maria/Applications/musrfit/src/include/PMusrCanvas.h"
    "/Users/maria/Applications/musrfit/src/include/PMusr.h"
    "/Users/maria/Applications/musrfit/src/include/PMusrT0.h"
    "/Users/maria/Applications/musrfit/src/include/PPrepFourier.h"
    "/Users/maria/Applications/musrfit/src/include/PRgeHandler.h"
    "/Users/maria/Applications/musrfit/src/include/PRunAsymmetry.h"
    "/Users/maria/Applications/musrfit/src/include/PRunAsymmetryBNMR.h"
    "/Users/maria/Applications/musrfit/src/include/PRunAsymmetryRRF.h"
    "/Users/maria/Applications/musrfit/src/include/PRunBase.h"
    "/Users/maria/Applications/musrfit/src/include/PRunDataHandler.h"
    "/Users/maria/Applications/musrfit/src/include/PRunListCollection.h"
    "/Users/maria/Applications/musrfit/src/include/PRunMuMinus.h"
    "/Users/maria/Applications/musrfit/src/include/PRunNonMusr.h"
    "/Users/maria/Applications/musrfit/src/include/PRunSingleHisto.h"
    "/Users/maria/Applications/musrfit/src/include/PRunSingleHistoRRF.h"
    "/Users/maria/Applications/musrfit/src/include/PStartupHandler.h"
    "/Users/maria/Applications/musrfit/src/include/PTheory.h"
    "/Users/maria/Applications/musrfit/src/include/PUserFcnBase.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig" TYPE FILE FILES
    "/Users/maria/Applications/musrfit/build/src/classes/PUserFcnBase.pc"
    "/Users/maria/Applications/musrfit/build/src/classes/PRgeHandler.pc"
    "/Users/maria/Applications/musrfit/build/src/classes/PMusr.pc"
    )
endif()

