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
include src/CMakeFiles/msr2msr.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include src/CMakeFiles/msr2msr.dir/compiler_depend.make

# Include the progress variables for this target.
include src/CMakeFiles/msr2msr.dir/progress.make

# Include the compile flags for this target's objects.
include src/CMakeFiles/msr2msr.dir/flags.make

src/CMakeFiles/msr2msr.dir/msr2msr.cpp.o: src/CMakeFiles/msr2msr.dir/flags.make
src/CMakeFiles/msr2msr.dir/msr2msr.cpp.o: /Users/maria/Applications/musrfit/src/msr2msr.cpp
src/CMakeFiles/msr2msr.dir/msr2msr.cpp.o: src/CMakeFiles/msr2msr.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/maria/Applications/musrfit/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object src/CMakeFiles/msr2msr.dir/msr2msr.cpp.o"
	cd /Users/maria/Applications/musrfit/build/src && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT src/CMakeFiles/msr2msr.dir/msr2msr.cpp.o -MF CMakeFiles/msr2msr.dir/msr2msr.cpp.o.d -o CMakeFiles/msr2msr.dir/msr2msr.cpp.o -c /Users/maria/Applications/musrfit/src/msr2msr.cpp

src/CMakeFiles/msr2msr.dir/msr2msr.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/msr2msr.dir/msr2msr.cpp.i"
	cd /Users/maria/Applications/musrfit/build/src && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/maria/Applications/musrfit/src/msr2msr.cpp > CMakeFiles/msr2msr.dir/msr2msr.cpp.i

src/CMakeFiles/msr2msr.dir/msr2msr.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/msr2msr.dir/msr2msr.cpp.s"
	cd /Users/maria/Applications/musrfit/build/src && /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/maria/Applications/musrfit/src/msr2msr.cpp -o CMakeFiles/msr2msr.dir/msr2msr.cpp.s

# Object files for target msr2msr
msr2msr_OBJECTS = \
"CMakeFiles/msr2msr.dir/msr2msr.cpp.o"

# External object files for target msr2msr
msr2msr_EXTERNAL_OBJECTS =

src/msr2msr: src/CMakeFiles/msr2msr.dir/msr2msr.cpp.o
src/msr2msr: src/CMakeFiles/msr2msr.dir/build.make
src/msr2msr: /opt/homebrew/Cellar/root/6.26.06_2/lib/root/libCore.so
src/msr2msr: /opt/homebrew/Cellar/root/6.26.06_2/lib/root/libImt.so
src/msr2msr: /opt/homebrew/Cellar/root/6.26.06_2/lib/root/libRIO.so
src/msr2msr: /opt/homebrew/Cellar/root/6.26.06_2/lib/root/libNet.so
src/msr2msr: /opt/homebrew/Cellar/root/6.26.06_2/lib/root/libHist.so
src/msr2msr: /opt/homebrew/Cellar/root/6.26.06_2/lib/root/libGraf.so
src/msr2msr: /opt/homebrew/Cellar/root/6.26.06_2/lib/root/libGraf3d.so
src/msr2msr: /opt/homebrew/Cellar/root/6.26.06_2/lib/root/libGpad.so
src/msr2msr: /opt/homebrew/Cellar/root/6.26.06_2/lib/root/libROOTDataFrame.so
src/msr2msr: /opt/homebrew/Cellar/root/6.26.06_2/lib/root/libTree.so
src/msr2msr: /opt/homebrew/Cellar/root/6.26.06_2/lib/root/libTreePlayer.so
src/msr2msr: /opt/homebrew/Cellar/root/6.26.06_2/lib/root/libRint.so
src/msr2msr: /opt/homebrew/Cellar/root/6.26.06_2/lib/root/libPostscript.so
src/msr2msr: /opt/homebrew/Cellar/root/6.26.06_2/lib/root/libMatrix.so
src/msr2msr: /opt/homebrew/Cellar/root/6.26.06_2/lib/root/libPhysics.so
src/msr2msr: /opt/homebrew/Cellar/root/6.26.06_2/lib/root/libMathCore.so
src/msr2msr: /opt/homebrew/Cellar/root/6.26.06_2/lib/root/libThread.so
src/msr2msr: /opt/homebrew/Cellar/root/6.26.06_2/lib/root/libMultiProc.so
src/msr2msr: /opt/homebrew/Cellar/root/6.26.06_2/lib/root/libROOTVecOps.so
src/msr2msr: /opt/homebrew/Cellar/root/6.26.06_2/lib/root/libGui.so
src/msr2msr: /opt/homebrew/Cellar/root/6.26.06_2/lib/root/libMathMore.so
src/msr2msr: /opt/homebrew/Cellar/root/6.26.06_2/lib/root/libMinuit2.so
src/msr2msr: /opt/homebrew/Cellar/root/6.26.06_2/lib/root/libXMLParser.so
src/msr2msr: src/CMakeFiles/msr2msr.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/maria/Applications/musrfit/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable msr2msr"
	cd /Users/maria/Applications/musrfit/build/src && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/msr2msr.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/CMakeFiles/msr2msr.dir/build: src/msr2msr
.PHONY : src/CMakeFiles/msr2msr.dir/build

src/CMakeFiles/msr2msr.dir/clean:
	cd /Users/maria/Applications/musrfit/build/src && $(CMAKE_COMMAND) -P CMakeFiles/msr2msr.dir/cmake_clean.cmake
.PHONY : src/CMakeFiles/msr2msr.dir/clean

src/CMakeFiles/msr2msr.dir/depend:
	cd /Users/maria/Applications/musrfit/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/maria/Applications/musrfit /Users/maria/Applications/musrfit/src /Users/maria/Applications/musrfit/build /Users/maria/Applications/musrfit/build/src /Users/maria/Applications/musrfit/build/src/CMakeFiles/msr2msr.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/CMakeFiles/msr2msr.dir/depend

