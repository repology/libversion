configure_file(config.h.in config.h @ONLY)

set(LIBVERSION_SOURCES
	compare.c
	signature.c
)

set(LIBVERSION_HEADERS
	version.h
)

add_library(version SHARED ${LIBVERSION_SOURCES} ${LIBVERSION_HEADERS})
target_include_directories(version PUBLIC ${PROJECT_SOURCE_DIR} ${PROJECT_BINARY_DIR})
set_target_properties(version PROPERTIES SOVERSION 0)
set_target_properties(version PROPERTIES VERSION 0.1.0)

add_library(version_static STATIC ${LIBVERSION_SOURCES} ${LIBVERSION_HEADERS})
target_include_directories(version_static PUBLIC ${PROJECT_SOURCE_DIR} ${PROJECT_BINARY_DIR})
set_target_properties(version_static PROPERTIES POSITION_INDEPENDENT_CODE ON)
set_target_properties(version_static PROPERTIES OUTPUT_NAME version)

configure_file(libversion.pc.in libversion.pc @ONLY)

install(FILES
	${LIBVERSION_HEADERS}
	${CMAKE_CURRENT_BINARY_DIR}/config.h
	DESTINATION include/libversion
)
install(TARGETS version version_static LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libversion.pc DESTINATION ${PKGCONFIGDIR})
