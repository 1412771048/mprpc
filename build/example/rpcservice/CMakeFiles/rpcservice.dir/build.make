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
CMAKE_SOURCE_DIR = /home/gyl/workspace/mprpc

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/gyl/workspace/mprpc/build

# Include any dependencies generated for this target.
include example/rpcservice/CMakeFiles/rpcservice.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include example/rpcservice/CMakeFiles/rpcservice.dir/compiler_depend.make

# Include the progress variables for this target.
include example/rpcservice/CMakeFiles/rpcservice.dir/progress.make

# Include the compile flags for this target's objects.
include example/rpcservice/CMakeFiles/rpcservice.dir/flags.make

example/rpcservice/CMakeFiles/rpcservice.dir/__/user.pb.cc.o: example/rpcservice/CMakeFiles/rpcservice.dir/flags.make
example/rpcservice/CMakeFiles/rpcservice.dir/__/user.pb.cc.o: ../example/user.pb.cc
example/rpcservice/CMakeFiles/rpcservice.dir/__/user.pb.cc.o: example/rpcservice/CMakeFiles/rpcservice.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/gyl/workspace/mprpc/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object example/rpcservice/CMakeFiles/rpcservice.dir/__/user.pb.cc.o"
	cd /home/gyl/workspace/mprpc/build/example/rpcservice && /usr/bin/ccache /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT example/rpcservice/CMakeFiles/rpcservice.dir/__/user.pb.cc.o -MF CMakeFiles/rpcservice.dir/__/user.pb.cc.o.d -o CMakeFiles/rpcservice.dir/__/user.pb.cc.o -c /home/gyl/workspace/mprpc/example/user.pb.cc

example/rpcservice/CMakeFiles/rpcservice.dir/__/user.pb.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/rpcservice.dir/__/user.pb.cc.i"
	cd /home/gyl/workspace/mprpc/build/example/rpcservice && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/gyl/workspace/mprpc/example/user.pb.cc > CMakeFiles/rpcservice.dir/__/user.pb.cc.i

example/rpcservice/CMakeFiles/rpcservice.dir/__/user.pb.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/rpcservice.dir/__/user.pb.cc.s"
	cd /home/gyl/workspace/mprpc/build/example/rpcservice && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/gyl/workspace/mprpc/example/user.pb.cc -o CMakeFiles/rpcservice.dir/__/user.pb.cc.s

example/rpcservice/CMakeFiles/rpcservice.dir/main.cpp.o: example/rpcservice/CMakeFiles/rpcservice.dir/flags.make
example/rpcservice/CMakeFiles/rpcservice.dir/main.cpp.o: ../example/rpcservice/main.cpp
example/rpcservice/CMakeFiles/rpcservice.dir/main.cpp.o: example/rpcservice/CMakeFiles/rpcservice.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/gyl/workspace/mprpc/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object example/rpcservice/CMakeFiles/rpcservice.dir/main.cpp.o"
	cd /home/gyl/workspace/mprpc/build/example/rpcservice && /usr/bin/ccache /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT example/rpcservice/CMakeFiles/rpcservice.dir/main.cpp.o -MF CMakeFiles/rpcservice.dir/main.cpp.o.d -o CMakeFiles/rpcservice.dir/main.cpp.o -c /home/gyl/workspace/mprpc/example/rpcservice/main.cpp

example/rpcservice/CMakeFiles/rpcservice.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/rpcservice.dir/main.cpp.i"
	cd /home/gyl/workspace/mprpc/build/example/rpcservice && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/gyl/workspace/mprpc/example/rpcservice/main.cpp > CMakeFiles/rpcservice.dir/main.cpp.i

example/rpcservice/CMakeFiles/rpcservice.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/rpcservice.dir/main.cpp.s"
	cd /home/gyl/workspace/mprpc/build/example/rpcservice && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/gyl/workspace/mprpc/example/rpcservice/main.cpp -o CMakeFiles/rpcservice.dir/main.cpp.s

# Object files for target rpcservice
rpcservice_OBJECTS = \
"CMakeFiles/rpcservice.dir/__/user.pb.cc.o" \
"CMakeFiles/rpcservice.dir/main.cpp.o"

# External object files for target rpcservice
rpcservice_EXTERNAL_OBJECTS =

../rpcservice: example/rpcservice/CMakeFiles/rpcservice.dir/__/user.pb.cc.o
../rpcservice: example/rpcservice/CMakeFiles/rpcservice.dir/main.cpp.o
../rpcservice: example/rpcservice/CMakeFiles/rpcservice.dir/build.make
../rpcservice: ../lib/libmprpc.so
../rpcservice: example/rpcservice/CMakeFiles/rpcservice.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/gyl/workspace/mprpc/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX executable ../../../rpcservice"
	cd /home/gyl/workspace/mprpc/build/example/rpcservice && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/rpcservice.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
example/rpcservice/CMakeFiles/rpcservice.dir/build: ../rpcservice
.PHONY : example/rpcservice/CMakeFiles/rpcservice.dir/build

example/rpcservice/CMakeFiles/rpcservice.dir/clean:
	cd /home/gyl/workspace/mprpc/build/example/rpcservice && $(CMAKE_COMMAND) -P CMakeFiles/rpcservice.dir/cmake_clean.cmake
.PHONY : example/rpcservice/CMakeFiles/rpcservice.dir/clean

example/rpcservice/CMakeFiles/rpcservice.dir/depend:
	cd /home/gyl/workspace/mprpc/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/gyl/workspace/mprpc /home/gyl/workspace/mprpc/example/rpcservice /home/gyl/workspace/mprpc/build /home/gyl/workspace/mprpc/build/example/rpcservice /home/gyl/workspace/mprpc/build/example/rpcservice/CMakeFiles/rpcservice.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : example/rpcservice/CMakeFiles/rpcservice.dir/depend
