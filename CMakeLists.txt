cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

# meta
project(libversion VERSION 2.9.1)

set(PKGCONFIGDIR lib/pkgconfig CACHE STRING "directory where to install pkg-config files")
if(CMAKE_SYSTEM_NAME MATCHES "FreeBSD" OR CMAKE_SYSTEM_NAME MATCHES "DragonFly")
	SET(PKGCONFIGDIR libdata/pkgconfig)
endif()

# subdirs
enable_testing()

add_subdirectory(libversion)
add_subdirectory(tests)

if(UNIX)
	add_subdirectory(version_compare)
	add_subdirectory(version_sort)
endif()
