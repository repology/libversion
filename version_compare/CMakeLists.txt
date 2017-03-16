ADD_EXECUTABLE(version_compare version_compare.c)
TARGET_LINK_LIBRARIES(version_compare version)
INSTALL(TARGETS version_compare RUNTIME DESTINATION bin)
