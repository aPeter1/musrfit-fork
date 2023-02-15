# Install script for directory: /Users/maria/Applications/musrfit/src

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
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "/Users/maria/Applications/musrfit/build/src/any2many")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/any2many" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/any2many")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/opt/homebrew/Cellar/root/6.26.06_2/lib/root"
      -delete_rpath "/Users/maria/Applications/musrfit/build/src/classes"
      -delete_rpath "/Users/maria/Applications/musrfit/build/src/external/mud/src"
      -delete_rpath "/Users/maria/Applications/musrfit/build/src/external/MusrRoot"
      -delete_rpath "/Users/maria/Applications/musrfit/build/src/external/TLemRunHeader"
      -delete_rpath "/Users/maria/Applications/musrfit/build/src/external/MuSR_software/Class_MuSR_PSI"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/any2many")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" -u -r "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/any2many")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "/Users/maria/Applications/musrfit/build/src/dump_header")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/dump_header" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/dump_header")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/opt/homebrew/Cellar/root/6.26.06_2/lib/root"
      -delete_rpath "/Users/maria/Applications/musrfit/build/src/classes"
      -delete_rpath "/Users/maria/Applications/musrfit/build/src/external/mud/src"
      -delete_rpath "/Users/maria/Applications/musrfit/build/src/external/MusrRoot"
      -delete_rpath "/Users/maria/Applications/musrfit/build/src/external/TLemRunHeader"
      -delete_rpath "/Users/maria/Applications/musrfit/build/src/external/MuSR_software/Class_MuSR_PSI"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/dump_header")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" -u -r "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/dump_header")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "/Users/maria/Applications/musrfit/build/src/msr2data")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/msr2data" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/msr2data")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/opt/homebrew/Cellar/root/6.26.06_2/lib/root"
      -delete_rpath "/Users/maria/Applications/musrfit/build/src/classes"
      -delete_rpath "/Users/maria/Applications/musrfit/build/src/external/mud/src"
      -delete_rpath "/Users/maria/Applications/musrfit/build/src/external/MusrRoot"
      -delete_rpath "/Users/maria/Applications/musrfit/build/src/external/TLemRunHeader"
      -delete_rpath "/Users/maria/Applications/musrfit/build/src/external/MuSR_software/Class_MuSR_PSI"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/msr2data")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" -u -r "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/msr2data")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "/Users/maria/Applications/musrfit/build/src/msr2msr")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/msr2msr" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/msr2msr")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/opt/homebrew/Cellar/root/6.26.06_2/lib/root"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/msr2msr")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" -u -r "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/msr2msr")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "/Users/maria/Applications/musrfit/build/src/musrfit")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/musrfit" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/musrfit")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/opt/homebrew/Cellar/root/6.26.06_2/lib/root"
      -delete_rpath "/Users/maria/Applications/musrfit/build/src/classes"
      -delete_rpath "/Users/maria/Applications/musrfit/build/src/external/mud/src"
      -delete_rpath "/Users/maria/Applications/musrfit/build/src/external/MusrRoot"
      -delete_rpath "/Users/maria/Applications/musrfit/build/src/external/TLemRunHeader"
      -delete_rpath "/Users/maria/Applications/musrfit/build/src/external/MuSR_software/Class_MuSR_PSI"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/musrfit")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" -u -r "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/musrfit")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "/Users/maria/Applications/musrfit/build/src/musrFT")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/musrFT" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/musrFT")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/opt/homebrew/Cellar/root/6.26.06_2/lib/root"
      -delete_rpath "/Users/maria/Applications/musrfit/build/src/classes"
      -delete_rpath "/Users/maria/Applications/musrfit/build/src/external/mud/src"
      -delete_rpath "/Users/maria/Applications/musrfit/build/src/external/MusrRoot"
      -delete_rpath "/Users/maria/Applications/musrfit/build/src/external/TLemRunHeader"
      -delete_rpath "/Users/maria/Applications/musrfit/build/src/external/MuSR_software/Class_MuSR_PSI"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/musrFT")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" -u -r "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/musrFT")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "/Users/maria/Applications/musrfit/build/src/musrRootValidation")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/musrRootValidation" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/musrRootValidation")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/opt/homebrew/Cellar/root/6.26.06_2/lib/root"
      -delete_rpath "/Users/maria/Applications/musrfit/build/src/classes"
      -delete_rpath "/Users/maria/Applications/musrfit/build/src/external/mud/src"
      -delete_rpath "/Users/maria/Applications/musrfit/build/src/external/MusrRoot"
      -delete_rpath "/Users/maria/Applications/musrfit/build/src/external/TLemRunHeader"
      -delete_rpath "/Users/maria/Applications/musrfit/build/src/external/MuSR_software/Class_MuSR_PSI"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/musrRootValidation")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" -u -r "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/musrRootValidation")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "/Users/maria/Applications/musrfit/build/src/musrt0")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/musrt0" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/musrt0")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/opt/homebrew/Cellar/root/6.26.06_2/lib/root"
      -delete_rpath "/Users/maria/Applications/musrfit/build/src/classes"
      -delete_rpath "/Users/maria/Applications/musrfit/build/src/external/mud/src"
      -delete_rpath "/Users/maria/Applications/musrfit/build/src/external/MusrRoot"
      -delete_rpath "/Users/maria/Applications/musrfit/build/src/external/TLemRunHeader"
      -delete_rpath "/Users/maria/Applications/musrfit/build/src/external/MuSR_software/Class_MuSR_PSI"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/musrt0")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" -u -r "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/musrt0")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "/Users/maria/Applications/musrfit/build/src/musrview")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/musrview" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/musrview")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/opt/homebrew/Cellar/root/6.26.06_2/lib/root"
      -delete_rpath "/Users/maria/Applications/musrfit/build/src/classes"
      -delete_rpath "/Users/maria/Applications/musrfit/build/src/external/mud/src"
      -delete_rpath "/Users/maria/Applications/musrfit/build/src/external/MusrRoot"
      -delete_rpath "/Users/maria/Applications/musrfit/build/src/external/TLemRunHeader"
      -delete_rpath "/Users/maria/Applications/musrfit/build/src/external/MuSR_software/Class_MuSR_PSI"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/musrview")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" -u -r "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/musrview")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "/Users/maria/Applications/musrfit/build/src/write_musrRoot_runHeader")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/write_musrRoot_runHeader" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/write_musrRoot_runHeader")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/opt/homebrew/Cellar/root/6.26.06_2/lib/root"
      -delete_rpath "/Users/maria/Applications/musrfit/build/src/classes"
      -delete_rpath "/Users/maria/Applications/musrfit/build/src/external/mud/src"
      -delete_rpath "/Users/maria/Applications/musrfit/build/src/external/MusrRoot"
      -delete_rpath "/Users/maria/Applications/musrfit/build/src/external/TLemRunHeader"
      -delete_rpath "/Users/maria/Applications/musrfit/build/src/external/MuSR_software/Class_MuSR_PSI"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/write_musrRoot_runHeader")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" -u -r "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/write_musrRoot_runHeader")
    endif()
  endif()
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/Users/maria/Applications/musrfit/build/src/classes/cmake_install.cmake")
  include("/Users/maria/Applications/musrfit/build/src/external/cmake_install.cmake")
  include("/Users/maria/Applications/musrfit/build/src/musredit_qt6/cmake_install.cmake")

endif()

