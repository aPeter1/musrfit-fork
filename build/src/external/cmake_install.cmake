# Install script for directory: /Users/maria/Applications/musrfit/src/external

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

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/Users/maria/Applications/musrfit/build/src/external/Nonlocal/cmake_install.cmake")
  include("/Users/maria/Applications/musrfit/build/src/external/MagProximity/cmake_install.cmake")
  include("/Users/maria/Applications/musrfit/build/src/external/libPhotoMeissner/cmake_install.cmake")
  include("/Users/maria/Applications/musrfit/build/src/external/libSpinValve/cmake_install.cmake")
  include("/Users/maria/Applications/musrfit/build/src/external/libGbGLF/cmake_install.cmake")
  include("/Users/maria/Applications/musrfit/build/src/external/DepthProfile/cmake_install.cmake")
  include("/Users/maria/Applications/musrfit/build/src/external/mud/cmake_install.cmake")
  include("/Users/maria/Applications/musrfit/build/src/external/MusrRoot/cmake_install.cmake")
  include("/Users/maria/Applications/musrfit/build/src/external/TLemRunHeader/cmake_install.cmake")
  include("/Users/maria/Applications/musrfit/build/src/external/MuSR_software/cmake_install.cmake")

endif()

