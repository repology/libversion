configure_file(config.h.in config.h @ONLY)

set(LIBVERSION_SOURCES
	compare.c
	signature.c
)

set(LIBVERSION_HEADERS
	version.h
)

add_library(version SHARED ${LIBVERSION_SOURCES} ${LIBVERSION_HEADERS})
target_include_directories(version PUBLIC
	$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
	$<BUILD_INTERFACE:${PROJECT_BINARY_DIR}>
	$<INSTALL_INTERFACE:include>
)
set_target_properties(version PROPERTIES
	SOVERSION 0
	VERSION 0.1.0
)

add_library(version_static STATIC ${LIBVERSION_SOURCES} ${LIBVERSION_HEADERS})
target_include_directories(version_static PUBLIC
	$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
	$<BUILD_INTERFACE:${PROJECT_BINARY_DIR}>
	$<INSTALL_INTERFACE:include>
)
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
install(TARGETS version version_static EXPORT libversion LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libversion.pc DESTINATION ${PKGCONFIGDIR})

install(EXPORT libversion NAMESPACE libversion:: DESTINATION lib/cmake/libversion FILE libversionConfig.cmake)
