cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

# meta
project(libversion VERSION 3.0.2)

include(GNUInstallDirs)

# subdirs
enable_testing()

add_subdirectory(libversion)
add_subdirectory(tests)

if(UNIX)
	add_subdirectory(utils)
endif()
