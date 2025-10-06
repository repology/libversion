cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)

# meta
project(libversion VERSION 3.0.4)

include(GNUInstallDirs)
enable_testing()

# subdirs
add_subdirectory(libversion)
add_subdirectory(tests)
add_subdirectory(utils)
