add_executable(version_compare version_compare.c)
target_link_libraries(version_compare libversion)
set_target_properties(version_compare PROPERTIES COMPILE_DEFINITIONS LIBVERSION_NO_DEPRECATED)
install(TARGETS version_compare)
