# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.18

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
CMAKE_SOURCE_DIR = /tmp/tmp.ZW79XHXcDm

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /tmp/tmp.ZW79XHXcDm/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/downloadManager.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/downloadManager.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/downloadManager.dir/flags.make

CMakeFiles/downloadManager.dir/main.c.o: CMakeFiles/downloadManager.dir/flags.make
CMakeFiles/downloadManager.dir/main.c.o: ../main.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/tmp/tmp.ZW79XHXcDm/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/downloadManager.dir/main.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/downloadManager.dir/main.c.o -c /tmp/tmp.ZW79XHXcDm/main.c

CMakeFiles/downloadManager.dir/main.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/downloadManager.dir/main.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /tmp/tmp.ZW79XHXcDm/main.c > CMakeFiles/downloadManager.dir/main.c.i

CMakeFiles/downloadManager.dir/main.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/downloadManager.dir/main.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /tmp/tmp.ZW79XHXcDm/main.c -o CMakeFiles/downloadManager.dir/main.c.s

CMakeFiles/downloadManager.dir/client/client.c.o: CMakeFiles/downloadManager.dir/flags.make
CMakeFiles/downloadManager.dir/client/client.c.o: ../client/client.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/tmp/tmp.ZW79XHXcDm/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object CMakeFiles/downloadManager.dir/client/client.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/downloadManager.dir/client/client.c.o -c /tmp/tmp.ZW79XHXcDm/client/client.c

CMakeFiles/downloadManager.dir/client/client.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/downloadManager.dir/client/client.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /tmp/tmp.ZW79XHXcDm/client/client.c > CMakeFiles/downloadManager.dir/client/client.c.i

CMakeFiles/downloadManager.dir/client/client.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/downloadManager.dir/client/client.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /tmp/tmp.ZW79XHXcDm/client/client.c -o CMakeFiles/downloadManager.dir/client/client.c.s

CMakeFiles/downloadManager.dir/server/server.c.o: CMakeFiles/downloadManager.dir/flags.make
CMakeFiles/downloadManager.dir/server/server.c.o: ../server/server.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/tmp/tmp.ZW79XHXcDm/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building C object CMakeFiles/downloadManager.dir/server/server.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/downloadManager.dir/server/server.c.o -c /tmp/tmp.ZW79XHXcDm/server/server.c

CMakeFiles/downloadManager.dir/server/server.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/downloadManager.dir/server/server.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /tmp/tmp.ZW79XHXcDm/server/server.c > CMakeFiles/downloadManager.dir/server/server.c.i

CMakeFiles/downloadManager.dir/server/server.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/downloadManager.dir/server/server.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /tmp/tmp.ZW79XHXcDm/server/server.c -o CMakeFiles/downloadManager.dir/server/server.c.s

CMakeFiles/downloadManager.dir/clientServer/client.c.o: CMakeFiles/downloadManager.dir/flags.make
CMakeFiles/downloadManager.dir/clientServer/client.c.o: ../clientServer/client.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/tmp/tmp.ZW79XHXcDm/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building C object CMakeFiles/downloadManager.dir/clientServer/client.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/downloadManager.dir/clientServer/client.c.o -c /tmp/tmp.ZW79XHXcDm/clientServer/client.c

CMakeFiles/downloadManager.dir/clientServer/client.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/downloadManager.dir/clientServer/client.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /tmp/tmp.ZW79XHXcDm/clientServer/client.c > CMakeFiles/downloadManager.dir/clientServer/client.c.i

CMakeFiles/downloadManager.dir/clientServer/client.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/downloadManager.dir/clientServer/client.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /tmp/tmp.ZW79XHXcDm/clientServer/client.c -o CMakeFiles/downloadManager.dir/clientServer/client.c.s

CMakeFiles/downloadManager.dir/clientServer/server.c.o: CMakeFiles/downloadManager.dir/flags.make
CMakeFiles/downloadManager.dir/clientServer/server.c.o: ../clientServer/server.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/tmp/tmp.ZW79XHXcDm/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building C object CMakeFiles/downloadManager.dir/clientServer/server.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/downloadManager.dir/clientServer/server.c.o -c /tmp/tmp.ZW79XHXcDm/clientServer/server.c

CMakeFiles/downloadManager.dir/clientServer/server.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/downloadManager.dir/clientServer/server.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /tmp/tmp.ZW79XHXcDm/clientServer/server.c > CMakeFiles/downloadManager.dir/clientServer/server.c.i

CMakeFiles/downloadManager.dir/clientServer/server.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/downloadManager.dir/clientServer/server.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /tmp/tmp.ZW79XHXcDm/clientServer/server.c -o CMakeFiles/downloadManager.dir/clientServer/server.c.s

# Object files for target downloadManager
downloadManager_OBJECTS = \
"CMakeFiles/downloadManager.dir/main.c.o" \
"CMakeFiles/downloadManager.dir/client/client.c.o" \
"CMakeFiles/downloadManager.dir/server/server.c.o" \
"CMakeFiles/downloadManager.dir/clientServer/client.c.o" \
"CMakeFiles/downloadManager.dir/clientServer/server.c.o"

# External object files for target downloadManager
downloadManager_EXTERNAL_OBJECTS =

downloadManager: CMakeFiles/downloadManager.dir/main.c.o
downloadManager: CMakeFiles/downloadManager.dir/client/client.c.o
downloadManager: CMakeFiles/downloadManager.dir/server/server.c.o
downloadManager: CMakeFiles/downloadManager.dir/clientServer/client.c.o
downloadManager: CMakeFiles/downloadManager.dir/clientServer/server.c.o
downloadManager: CMakeFiles/downloadManager.dir/build.make
downloadManager: CMakeFiles/downloadManager.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/tmp/tmp.ZW79XHXcDm/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Linking C executable downloadManager"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/downloadManager.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/downloadManager.dir/build: downloadManager

.PHONY : CMakeFiles/downloadManager.dir/build

CMakeFiles/downloadManager.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/downloadManager.dir/cmake_clean.cmake
.PHONY : CMakeFiles/downloadManager.dir/clean

CMakeFiles/downloadManager.dir/depend:
	cd /tmp/tmp.ZW79XHXcDm/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /tmp/tmp.ZW79XHXcDm /tmp/tmp.ZW79XHXcDm /tmp/tmp.ZW79XHXcDm/cmake-build-debug /tmp/tmp.ZW79XHXcDm/cmake-build-debug /tmp/tmp.ZW79XHXcDm/cmake-build-debug/CMakeFiles/downloadManager.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/downloadManager.dir/depend

