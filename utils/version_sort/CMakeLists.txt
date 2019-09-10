set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS NO)

add_executable(version_sort version_sort.cc)
target_link_libraries(version_sort libversion)
set_target_properties(version_sort PROPERTIES COMPILE_DEFINITIONS LIBVERSION_NO_DEPRECATED)
install(TARGETS version_sort RUNTIME DESTINATION bin)
