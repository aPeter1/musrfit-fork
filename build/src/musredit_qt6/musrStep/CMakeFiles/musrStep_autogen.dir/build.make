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

# Utility rule file for musrStep_autogen.

# Include any custom commands dependencies for this target.
include src/musredit_qt6/musrStep/CMakeFiles/musrStep_autogen.dir/compiler_depend.make

# Include the progress variables for this target.
include src/musredit_qt6/musrStep/CMakeFiles/musrStep_autogen.dir/progress.make

src/musredit_qt6/musrStep/CMakeFiles/musrStep_autogen:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/Users/maria/Applications/musrfit/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Automatic MOC for target musrStep"
	cd /Users/maria/Applications/musrfit/build/src/musredit_qt6/musrStep && /opt/homebrew/Cellar/cmake/3.25.1/bin/cmake -E cmake_autogen /Users/maria/Applications/musrfit/build/src/musredit_qt6/musrStep/CMakeFiles/musrStep_autogen.dir/AutogenInfo.json Release

musrStep_autogen: src/musredit_qt6/musrStep/CMakeFiles/musrStep_autogen
musrStep_autogen: src/musredit_qt6/musrStep/CMakeFiles/musrStep_autogen.dir/build.make
.PHONY : musrStep_autogen

# Rule to build all files generated by this target.
src/musredit_qt6/musrStep/CMakeFiles/musrStep_autogen.dir/build: musrStep_autogen
.PHONY : src/musredit_qt6/musrStep/CMakeFiles/musrStep_autogen.dir/build

src/musredit_qt6/musrStep/CMakeFiles/musrStep_autogen.dir/clean:
	cd /Users/maria/Applications/musrfit/build/src/musredit_qt6/musrStep && $(CMAKE_COMMAND) -P CMakeFiles/musrStep_autogen.dir/cmake_clean.cmake
.PHONY : src/musredit_qt6/musrStep/CMakeFiles/musrStep_autogen.dir/clean

src/musredit_qt6/musrStep/CMakeFiles/musrStep_autogen.dir/depend:
	cd /Users/maria/Applications/musrfit/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/maria/Applications/musrfit /Users/maria/Applications/musrfit/src/musredit_qt6/musrStep /Users/maria/Applications/musrfit/build /Users/maria/Applications/musrfit/build/src/musredit_qt6/musrStep /Users/maria/Applications/musrfit/build/src/musredit_qt6/musrStep/CMakeFiles/musrStep_autogen.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/musredit_qt6/musrStep/CMakeFiles/musrStep_autogen.dir/depend

