cmake_minimum_required(VERSION 3.0)
find_package(libversion REQUIRED)
add_executable(test test.c)
target_link_libraries(test libversion::libversion)
