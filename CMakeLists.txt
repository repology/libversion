cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

# meta
project(libversion VERSION 3.0.2)

include(GNUInstallDirs)
enable_testing()

# subdirs
add_subdirectory(libversion)
add_subdirectory(tests)
add_subdirectory(utils)
