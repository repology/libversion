include(GenerateExportHeader)

configure_file(config.h.in config.h @ONLY)

set(LIBVERSION_SOURCES
	compare.c
	signature.c
)

set(LIBVERSION_HEADERS
	version.h
)

add_library(libversion SHARED ${LIBVERSION_SOURCES} ${LIBVERSION_HEADERS})
target_include_directories(libversion PUBLIC
	$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
	$<BUILD_INTERFACE:${PROJECT_BINARY_DIR}>
	$<INSTALL_INTERFACE:include>
)
set_target_properties(libversion PROPERTIES
	SOVERSION 0
	VERSION 0.1.0
	OUTPUT_NAME version
)
generate_export_header(libversion EXPORT_FILE_NAME export.h)

add_library(libversion_static STATIC ${LIBVERSION_SOURCES} ${LIBVERSION_HEADERS})
target_include_directories(libversion_static PUBLIC
	$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
	$<BUILD_INTERFACE:${PROJECT_BINARY_DIR}>
	$<INSTALL_INTERFACE:include>
)
set_target_properties(libversion_static PROPERTIES
	POSITION_INDEPENDENT_CODE ON
	OUTPUT_NAME version
)

configure_file(libversion.pc.in libversion.pc @ONLY)

install(FILES
	${LIBVERSION_HEADERS}
	${CMAKE_CURRENT_BINARY_DIR}/config.h
	${CMAKE_CURRENT_BINARY_DIR}/export.h
	DESTINATION include/libversion
)
install(TARGETS libversion libversion_static EXPORT libversion LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libversion.pc DESTINATION ${PKGCONFIGDIR})

install(EXPORT libversion NAMESPACE libversion:: DESTINATION lib/cmake/libversion FILE libversionConfig.cmake)
