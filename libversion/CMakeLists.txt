set(LIBVERSION_SOURCES
	compare.c
	signature.c
)

set(LIBVERSION_HEADERS
	version.h
)

configure_file(config.h.in config.h @ONLY)

add_library(version SHARED ${LIBVERSION_SOURCES} ${LIBVERSION_HEADERS})
set_target_properties(version PROPERTIES
	SOVERSION 0
	VERSION 0.1.0
)

add_library(version_static STATIC ${LIBVERSION_SOURCES} ${LIBVERSION_HEADERS})
set_target_properties(version_static PROPERTIES
	POSITION_INDEPENDENT_CODE ON
	OUTPUT_NAME version
)

configure_file(libversion.pc.in libversion.pc @ONLY)

install(FILES
	${LIBVERSION_HEADERS}
	${CMAKE_CURRENT_BINARY_DIR}/config.h
	DESTINATION include/libversion
)
install(TARGETS version version_static LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libversion.pc DESTINATION ${PKGCONFIGDIR})
