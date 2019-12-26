cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

# meta
project(libversion VERSION 3.0.1)

set(PKGCONFIGDIR lib/pkgconfig CACHE STRING "directory where to install pkg-config files")
if(CMAKE_SYSTEM_NAME MATCHES "FreeBSD" OR CMAKE_SYSTEM_NAME MATCHES "DragonFly")
	SET(PKGCONFIGDIR libdata/pkgconfig)
endif()

# subdirs
enable_testing()

add_subdirectory(libversion)
add_subdirectory(tests)

if(UNIX)
	add_subdirectory(utils)
endif()
