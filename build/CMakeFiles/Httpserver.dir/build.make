# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/agni/Desktop/httpserver_edit

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/agni/Desktop/httpserver_edit/build

# Include any dependencies generated for this target.
include CMakeFiles/Httpserver.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/Httpserver.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/Httpserver.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/Httpserver.dir/flags.make

CMakeFiles/Httpserver.dir/src/main.cpp.o: CMakeFiles/Httpserver.dir/flags.make
CMakeFiles/Httpserver.dir/src/main.cpp.o: ../src/main.cpp
CMakeFiles/Httpserver.dir/src/main.cpp.o: CMakeFiles/Httpserver.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/agni/Desktop/httpserver_edit/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/Httpserver.dir/src/main.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/Httpserver.dir/src/main.cpp.o -MF CMakeFiles/Httpserver.dir/src/main.cpp.o.d -o CMakeFiles/Httpserver.dir/src/main.cpp.o -c /home/agni/Desktop/httpserver_edit/src/main.cpp

CMakeFiles/Httpserver.dir/src/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Httpserver.dir/src/main.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/agni/Desktop/httpserver_edit/src/main.cpp > CMakeFiles/Httpserver.dir/src/main.cpp.i

CMakeFiles/Httpserver.dir/src/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Httpserver.dir/src/main.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/agni/Desktop/httpserver_edit/src/main.cpp -o CMakeFiles/Httpserver.dir/src/main.cpp.s

# Object files for target Httpserver
Httpserver_OBJECTS = \
"CMakeFiles/Httpserver.dir/src/main.cpp.o"

# External object files for target Httpserver
Httpserver_EXTERNAL_OBJECTS =

../out/Httpserver: CMakeFiles/Httpserver.dir/src/main.cpp.o
../out/Httpserver: CMakeFiles/Httpserver.dir/build.make
../out/Httpserver: CMakeFiles/Httpserver.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/agni/Desktop/httpserver_edit/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable ../out/Httpserver"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/Httpserver.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/Httpserver.dir/build: ../out/Httpserver
.PHONY : CMakeFiles/Httpserver.dir/build

CMakeFiles/Httpserver.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/Httpserver.dir/cmake_clean.cmake
.PHONY : CMakeFiles/Httpserver.dir/clean

CMakeFiles/Httpserver.dir/depend:
	cd /home/agni/Desktop/httpserver_edit/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/agni/Desktop/httpserver_edit /home/agni/Desktop/httpserver_edit /home/agni/Desktop/httpserver_edit/build /home/agni/Desktop/httpserver_edit/build /home/agni/Desktop/httpserver_edit/build/CMakeFiles/Httpserver.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/Httpserver.dir/depend
