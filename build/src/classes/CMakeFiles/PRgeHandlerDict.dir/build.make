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

# Utility rule file for PRgeHandlerDict.

# Include any custom commands dependencies for this target.
include src/classes/CMakeFiles/PRgeHandlerDict.dir/compiler_depend.make

# Include the progress variables for this target.
include src/classes/CMakeFiles/PRgeHandlerDict.dir/progress.make

src/classes/CMakeFiles/PRgeHandlerDict: src/classes/PRgeHandlerDict.cxx
src/classes/CMakeFiles/PRgeHandlerDict: src/classes/libPRgeHandler_rdict.pcm
src/classes/CMakeFiles/PRgeHandlerDict: src/classes/libPRgeHandler.rootmap

src/classes/PRgeHandlerDict.cxx: /Users/maria/Applications/musrfit/src/include/PRgeHandlerLinkDef.h
src/classes/PRgeHandlerDict.cxx: /Users/maria/Applications/musrfit/src/include/PRgeHandler.h
src/classes/PRgeHandlerDict.cxx: /Users/maria/Applications/musrfit/src/include/PRgeHandler.h
src/classes/PRgeHandlerDict.cxx: /Users/maria/Applications/musrfit/src/include/PRgeHandlerLinkDef.h
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/Users/maria/Applications/musrfit/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Generating PRgeHandlerDict.cxx, libPRgeHandler_rdict.pcm, libPRgeHandler.rootmap"
	cd /Users/maria/Applications/musrfit/build/src/classes && /opt/homebrew/Cellar/cmake/3.25.1/bin/cmake -E env LD_LIBRARY_PATH=/opt/homebrew/Cellar/root/6.26.06_2/lib/root: /opt/homebrew/bin/rootcling -v2 -f PRgeHandlerDict.cxx -s /Users/maria/Applications/musrfit/build/src/classes/libPRgeHandler.dylib -rml libPRgeHandler.dylib -rmf /Users/maria/Applications/musrfit/build/src/classes/libPRgeHandler.rootmap -I -I/Users/maria/Applications/musrfit/src/include -I/opt/homebrew/include -I/Users/maria/Applications/musrfit/src/classes -inlineInputHeader -compilerI/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX13.1.sdk/usr/include/c++/v1 -compilerI/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/lib/clang/14.0.0/include -compilerI/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX13.1.sdk/usr/include -compilerI/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/include -compilerI/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/lib/clang/14.0.0/include -compilerI/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX13.1.sdk/usr/include -compilerI/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/include -I/opt/homebrew/Cellar/root/6.26.06_2/include/root -I/Users/maria/Applications/musrfit/src/include -I/Users/maria/Applications/musrfit/src/classes PRgeHandler.h /Users/maria/Applications/musrfit/src/include/PRgeHandlerLinkDef.h

src/classes/libPRgeHandler_rdict.pcm: src/classes/PRgeHandlerDict.cxx
	@$(CMAKE_COMMAND) -E touch_nocreate src/classes/libPRgeHandler_rdict.pcm

src/classes/libPRgeHandler.rootmap: src/classes/PRgeHandlerDict.cxx
	@$(CMAKE_COMMAND) -E touch_nocreate src/classes/libPRgeHandler.rootmap

PRgeHandlerDict: src/classes/CMakeFiles/PRgeHandlerDict
PRgeHandlerDict: src/classes/PRgeHandlerDict.cxx
PRgeHandlerDict: src/classes/libPRgeHandler.rootmap
PRgeHandlerDict: src/classes/libPRgeHandler_rdict.pcm
PRgeHandlerDict: src/classes/CMakeFiles/PRgeHandlerDict.dir/build.make
.PHONY : PRgeHandlerDict

# Rule to build all files generated by this target.
src/classes/CMakeFiles/PRgeHandlerDict.dir/build: PRgeHandlerDict
.PHONY : src/classes/CMakeFiles/PRgeHandlerDict.dir/build

src/classes/CMakeFiles/PRgeHandlerDict.dir/clean:
	cd /Users/maria/Applications/musrfit/build/src/classes && $(CMAKE_COMMAND) -P CMakeFiles/PRgeHandlerDict.dir/cmake_clean.cmake
.PHONY : src/classes/CMakeFiles/PRgeHandlerDict.dir/clean

src/classes/CMakeFiles/PRgeHandlerDict.dir/depend:
	cd /Users/maria/Applications/musrfit/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/maria/Applications/musrfit /Users/maria/Applications/musrfit/src/classes /Users/maria/Applications/musrfit/build /Users/maria/Applications/musrfit/build/src/classes /Users/maria/Applications/musrfit/build/src/classes/CMakeFiles/PRgeHandlerDict.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/classes/CMakeFiles/PRgeHandlerDict.dir/depend

