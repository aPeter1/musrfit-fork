# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.25

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /opt/homebrew/Cellar/cmake/3.25.1/bin/cmake

# The command to remove a file.
RM = /opt/homebrew/Cellar/cmake/3.25.1/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/maria/Applications/musrfit

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/maria/Applications/musrfit/build

# Include any dependencies generated for this target.
include src/classes/CMakeFiles/PUserFcnBase.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include src/classes/CMakeFiles/PUserFcnBase.dir/compiler_depend.make

# Include the progress variables for this target.
include src/classes/CMakeFiles/PUserFcnBase.dir/progress.make

# Include the compile flags for this target's objects.
include src/classes/CMakeFiles/PUserFcnBase.dir/flags.make

src/classes/PUserFcnBaseDict.cxx: /Users/maria/Applications/musrfit/src/include/PUserFcnBaseLinkDef.h
src/classes/PUserFcnBaseDict.cxx: /Users/maria/Applications/musrfit/src/include/PUserFcnBase.h
src/classes/PUserFcnBaseDict.cxx: /Users/maria/Applications/musrfit/src/include/PUserFcnBase.h
src/classes/PUserFcnBaseDict.cxx: /Users/maria/Applications/musrfit/src/include/PUserFcnBaseLinkDef.h
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/Users/maria/Applications/musrfit/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Generating PUserFcnBaseDict.cxx, libPUserFcnBase_rdict.pcm, libPUserFcnBase.rootmap"
	cd /Users/maria/Applications/musrfit/build/src/classes && /opt/homebrew/Cellar/cmake/3.25.1/bin/cmake -E env LD_LIBRARY_PATH=/opt/homebrew/Cellar/root/6.26.06_2/lib/root: /opt/homebrew/bin/rootcling -v2 -f PUserFcnBaseDict.cxx -s /Users/maria/Applications/musrfit/build/src/classes/libPUserFcnBase.dylib -rml libPUserFcnBase.dylib -rmf /Users/maria/Applications/musrfit/build/src/classes/libPUserFcnBase.rootmap -inlineInputHeader -I/opt/homebrew/include -I/Users/maria/Applications/musrfit/src/include -compilerI/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX13.1.sdk/usr/include/c++/v1 -compilerI/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/lib/clang/14.0.0/include -compilerI/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX13.1.sdk/usr/include -compilerI/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/include -compilerI/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/lib/clang/14.0.0/include -compilerI/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX13.1.sdk/usr/include -compilerI/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/include -I/opt/homebrew/Cellar/root/6.26.06_2/include/root -I/Users/maria/Applications/musrfit/src/include -I/Users/maria/Applications/musrfit/src/classes PUserFcnBase.h /Users/maria/Applications/musrfit/src/include/PUserFcnBaseLinkDef.h

src/classes/libPUserFcnBase_rdict.pcm: src/classes/PUserFcnBaseDict.cxx
	@$(CMAKE_COMMAND) -E touch_nocreate src/classes/libPUserFcnBase_rdict.pcm

src/classes/libPUserFcnBase.rootmap: src/classes/PUserFcnBaseDict.cxx
	@$(CMAKE_COMMAND) -E touch_nocreate src/classes/libPUserFcnBase.rootmap

src/classes/CMakeFiles/PUserFcnBase.dir/PUserFcnBase.cpp.o: src/classes/CMakeFiles/PUserFcnBase.dir/flags.make
src/classes/CMakeFiles/PUserFcnBase.dir/PUserFcnBase.cpp.o: /Users/maria/Applications/musrfit/src/classes/PUserFcnBase.cpp
src/classes/CMakeFiles/PUserFcnBase.dir/PUserFcnBase.cpp.o: src/classes/CMakeFiles/PUserFcnBase.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/maria/Applications/musrfit/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object src/classes/CMakeFiles/PUserFcnBase.dir/PUserFcnBase.cpp.o"
	cd /Users/maria/Applications/musrfit/build/src/classes && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT src/classes/CMakeFiles/PUserFcnBase.dir/PUserFcnBase.cpp.o -MF CMakeFiles/PUserFcnBase.dir/PUserFcnBase.cpp.o.d -o CMakeFiles/PUserFcnBase.dir/PUserFcnBase.cpp.o -c /Users/maria/Applications/musrfit/src/classes/PUserFcnBase.cpp

src/classes/CMakeFiles/PUserFcnBase.dir/PUserFcnBase.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/PUserFcnBase.dir/PUserFcnBase.cpp.i"
	cd /Users/maria/Applications/musrfit/build/src/classes && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/maria/Applications/musrfit/src/classes/PUserFcnBase.cpp > CMakeFiles/PUserFcnBase.dir/PUserFcnBase.cpp.i

src/classes/CMakeFiles/PUserFcnBase.dir/PUserFcnBase.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/PUserFcnBase.dir/PUserFcnBase.cpp.s"
	cd /Users/maria/Applications/musrfit/build/src/classes && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/maria/Applications/musrfit/src/classes/PUserFcnBase.cpp -o CMakeFiles/PUserFcnBase.dir/PUserFcnBase.cpp.s

src/classes/CMakeFiles/PUserFcnBase.dir/PUserFcnBaseDict.cxx.o: src/classes/CMakeFiles/PUserFcnBase.dir/flags.make
src/classes/CMakeFiles/PUserFcnBase.dir/PUserFcnBaseDict.cxx.o: src/classes/PUserFcnBaseDict.cxx
src/classes/CMakeFiles/PUserFcnBase.dir/PUserFcnBaseDict.cxx.o: src/classes/CMakeFiles/PUserFcnBase.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/maria/Applications/musrfit/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object src/classes/CMakeFiles/PUserFcnBase.dir/PUserFcnBaseDict.cxx.o"
	cd /Users/maria/Applications/musrfit/build/src/classes && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT src/classes/CMakeFiles/PUserFcnBase.dir/PUserFcnBaseDict.cxx.o -MF CMakeFiles/PUserFcnBase.dir/PUserFcnBaseDict.cxx.o.d -o CMakeFiles/PUserFcnBase.dir/PUserFcnBaseDict.cxx.o -c /Users/maria/Applications/musrfit/build/src/classes/PUserFcnBaseDict.cxx

src/classes/CMakeFiles/PUserFcnBase.dir/PUserFcnBaseDict.cxx.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/PUserFcnBase.dir/PUserFcnBaseDict.cxx.i"
	cd /Users/maria/Applications/musrfit/build/src/classes && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/maria/Applications/musrfit/build/src/classes/PUserFcnBaseDict.cxx > CMakeFiles/PUserFcnBase.dir/PUserFcnBaseDict.cxx.i

src/classes/CMakeFiles/PUserFcnBase.dir/PUserFcnBaseDict.cxx.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/PUserFcnBase.dir/PUserFcnBaseDict.cxx.s"
	cd /Users/maria/Applications/musrfit/build/src/classes && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/maria/Applications/musrfit/build/src/classes/PUserFcnBaseDict.cxx -o CMakeFiles/PUserFcnBase.dir/PUserFcnBaseDict.cxx.s

# Object files for target PUserFcnBase
PUserFcnBase_OBJECTS = \
"CMakeFiles/PUserFcnBase.dir/PUserFcnBase.cpp.o" \
"CMakeFiles/PUserFcnBase.dir/PUserFcnBaseDict.cxx.o"

# External object files for target PUserFcnBase
PUserFcnBase_EXTERNAL_OBJECTS =

src/classes/libPUserFcnBase.1.3.0.dylib: src/classes/CMakeFiles/PUserFcnBase.dir/PUserFcnBase.cpp.o
src/classes/libPUserFcnBase.1.3.0.dylib: src/classes/CMakeFiles/PUserFcnBase.dir/PUserFcnBaseDict.cxx.o
src/classes/libPUserFcnBase.1.3.0.dylib: src/classes/CMakeFiles/PUserFcnBase.dir/build.make
src/classes/libPUserFcnBase.1.3.0.dylib: /opt/homebrew/Cellar/root/6.26.06_2/lib/root/libCore.so
src/classes/libPUserFcnBase.1.3.0.dylib: /opt/homebrew/Cellar/root/6.26.06_2/lib/root/libImt.so
src/classes/libPUserFcnBase.1.3.0.dylib: /opt/homebrew/Cellar/root/6.26.06_2/lib/root/libRIO.so
src/classes/libPUserFcnBase.1.3.0.dylib: /opt/homebrew/Cellar/root/6.26.06_2/lib/root/libNet.so
src/classes/libPUserFcnBase.1.3.0.dylib: /opt/homebrew/Cellar/root/6.26.06_2/lib/root/libHist.so
src/classes/libPUserFcnBase.1.3.0.dylib: /opt/homebrew/Cellar/root/6.26.06_2/lib/root/libGraf.so
src/classes/libPUserFcnBase.1.3.0.dylib: /opt/homebrew/Cellar/root/6.26.06_2/lib/root/libGraf3d.so
src/classes/libPUserFcnBase.1.3.0.dylib: /opt/homebrew/Cellar/root/6.26.06_2/lib/root/libGpad.so
src/classes/libPUserFcnBase.1.3.0.dylib: /opt/homebrew/Cellar/root/6.26.06_2/lib/root/libROOTDataFrame.so
src/classes/libPUserFcnBase.1.3.0.dylib: /opt/homebrew/Cellar/root/6.26.06_2/lib/root/libTree.so
src/classes/libPUserFcnBase.1.3.0.dylib: /opt/homebrew/Cellar/root/6.26.06_2/lib/root/libTreePlayer.so
src/classes/libPUserFcnBase.1.3.0.dylib: /opt/homebrew/Cellar/root/6.26.06_2/lib/root/libRint.so
src/classes/libPUserFcnBase.1.3.0.dylib: /opt/homebrew/Cellar/root/6.26.06_2/lib/root/libPostscript.so
src/classes/libPUserFcnBase.1.3.0.dylib: /opt/homebrew/Cellar/root/6.26.06_2/lib/root/libMatrix.so
src/classes/libPUserFcnBase.1.3.0.dylib: /opt/homebrew/Cellar/root/6.26.06_2/lib/root/libPhysics.so
src/classes/libPUserFcnBase.1.3.0.dylib: /opt/homebrew/Cellar/root/6.26.06_2/lib/root/libMathCore.so
src/classes/libPUserFcnBase.1.3.0.dylib: /opt/homebrew/Cellar/root/6.26.06_2/lib/root/libThread.so
src/classes/libPUserFcnBase.1.3.0.dylib: /opt/homebrew/Cellar/root/6.26.06_2/lib/root/libMultiProc.so
src/classes/libPUserFcnBase.1.3.0.dylib: /opt/homebrew/Cellar/root/6.26.06_2/lib/root/libROOTVecOps.so
src/classes/libPUserFcnBase.1.3.0.dylib: /opt/homebrew/Cellar/root/6.26.06_2/lib/root/libGui.so
src/classes/libPUserFcnBase.1.3.0.dylib: /opt/homebrew/Cellar/root/6.26.06_2/lib/root/libMathMore.so
src/classes/libPUserFcnBase.1.3.0.dylib: /opt/homebrew/Cellar/root/6.26.06_2/lib/root/libMinuit2.so
src/classes/libPUserFcnBase.1.3.0.dylib: /opt/homebrew/Cellar/root/6.26.06_2/lib/root/libXMLParser.so
src/classes/libPUserFcnBase.1.3.0.dylib: src/classes/CMakeFiles/PUserFcnBase.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/maria/Applications/musrfit/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Linking CXX shared library libPUserFcnBase.dylib"
	cd /Users/maria/Applications/musrfit/build/src/classes && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/PUserFcnBase.dir/link.txt --verbose=$(VERBOSE)
	cd /Users/maria/Applications/musrfit/build/src/classes && $(CMAKE_COMMAND) -E cmake_symlink_library libPUserFcnBase.1.3.0.dylib libPUserFcnBase.1.3.0.dylib libPUserFcnBase.dylib

src/classes/libPUserFcnBase.dylib: src/classes/libPUserFcnBase.1.3.0.dylib
	@$(CMAKE_COMMAND) -E touch_nocreate src/classes/libPUserFcnBase.dylib

# Rule to build all files generated by this target.
src/classes/CMakeFiles/PUserFcnBase.dir/build: src/classes/libPUserFcnBase.dylib
.PHONY : src/classes/CMakeFiles/PUserFcnBase.dir/build

src/classes/CMakeFiles/PUserFcnBase.dir/clean:
	cd /Users/maria/Applications/musrfit/build/src/classes && $(CMAKE_COMMAND) -P CMakeFiles/PUserFcnBase.dir/cmake_clean.cmake
.PHONY : src/classes/CMakeFiles/PUserFcnBase.dir/clean

src/classes/CMakeFiles/PUserFcnBase.dir/depend: src/classes/PUserFcnBaseDict.cxx
src/classes/CMakeFiles/PUserFcnBase.dir/depend: src/classes/libPUserFcnBase.rootmap
src/classes/CMakeFiles/PUserFcnBase.dir/depend: src/classes/libPUserFcnBase_rdict.pcm
	cd /Users/maria/Applications/musrfit/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/maria/Applications/musrfit /Users/maria/Applications/musrfit/src/classes /Users/maria/Applications/musrfit/build /Users/maria/Applications/musrfit/build/src/classes /Users/maria/Applications/musrfit/build/src/classes/CMakeFiles/PUserFcnBase.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/classes/CMakeFiles/PUserFcnBase.dir/depend

