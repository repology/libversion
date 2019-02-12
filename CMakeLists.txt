project(libversion)

# meta
cmake_minimum_required(VERSION 3.0)

set(VERSION_MAJOR 2)
set(VERSION_MINOR 8)
set(VERSION_PATCH 2)

set(VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")

set(PKGCONFIGDIR lib/pkgconfig CACHE STRING "directory where to install pkg-config files")
if(CMAKE_SYSTEM_NAME MATCHES "FreeBSD" OR CMAKE_SYSTEM_NAME MATCHES "DragonFly")
	SET(PKGCONFIGDIR libdata/pkgconfig)
endif()

include_directories(BEFORE ${PROJECT_SOURCE_DIR} ${PROJECT_BINARY_DIR})

enable_testing()

# subdirs
add_subdirectory(libversion)
add_subdirectory(tests)

if(UNIX)
	add_subdirectory(version_compare)
endif()
