cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

# meta
project(libversion)

set(LIBVERSION_VERSION_MAJOR 2)
set(LIBVERSION_VERSION_MINOR 8)
set(LIBVERSION_VERSION_PATCH 2)

set(LIBVERSION_VERSION "${LIBVERSION_VERSION_MAJOR}.${LIBVERSION_VERSION_MINOR}.${LIBVERSION_VERSION_PATCH}")

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
endif()
