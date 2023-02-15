# Install script for directory: /Users/maria/Applications/musrfit/src/musredit_qt6/mupp/plotter

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
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/Applications/mupp.app/Contents/MacOS/mupp_plot")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "/Applications/mupp.app/Contents/MacOS" TYPE EXECUTABLE FILES "/Users/maria/Applications/musrfit/build/src/musredit_qt6/mupp/plotter/mupp_plot")
  if(EXISTS "$ENV{DESTDIR}/Applications/mupp.app/Contents/MacOS/mupp_plot" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/Applications/mupp.app/Contents/MacOS/mupp_plot")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/opt/homebrew/Cellar/root/6.26.06_2/lib/root"
      "$ENV{DESTDIR}/Applications/mupp.app/Contents/MacOS/mupp_plot")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" -u -r "$ENV{DESTDIR}/Applications/mupp.app/Contents/MacOS/mupp_plot")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE FILE FILES
    "/Users/maria/Applications/musrfit/build/src/musredit_qt6/mupp/plotter/libPMuppCanvas_rdict.pcm"
    "/Users/maria/Applications/musrfit/build/src/musredit_qt6/mupp/plotter/libPMuppCanvas.rootmap"
    "/Users/maria/Applications/musrfit/build/src/musredit_qt6/mupp/plotter/libPMuppStartupHandler_rdict.pcm"
    "/Users/maria/Applications/musrfit/build/src/musredit_qt6/mupp/plotter/libPMuppStartupHandler.rootmap"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE FILE FILES
    "/Users/maria/Applications/musrfit/src/musredit_qt6/mupp/plotter/../mupp.h"
    "/Users/maria/Applications/musrfit/src/musredit_qt6/mupp/plotter/mupp_plot.h"
    "/Users/maria/Applications/musrfit/src/musredit_qt6/mupp/plotter/PMuppCanvas.h"
    "/Users/maria/Applications/musrfit/src/musredit_qt6/mupp/plotter/PMuppStartupHandler.h"
    )
endif()

