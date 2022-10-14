cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

# meta
project(libversion VERSION 3.0.2)

include(GNUInstallDirs)

if(CMAKE_SYSTEM_NAME MATCHES "FreeBSD" OR CMAKE_SYSTEM_NAME MATCHES "DragonFly")
	set(CMAKE_INSTALL_PKGCONFIGDIR libdata/pkgconfig CACHE PATH "pkg-config files (libdata/pkgconfig)")
else()
	set(CMAKE_INSTALL_PKGCONFIGDIR ${CMAKE_INSTALL_LIBDIR}/pkgconfig CACHE PATH "pkg-config files (LIBDIR/pkgconfig)")
endif()
mark_as_advanced(CMAKE_INSTALL_PKGCONFIGDIR)

# subdirs
enable_testing()

add_subdirectory(libversion)
add_subdirectory(tests)

if(UNIX)
	add_subdirectory(utils)
endif()
