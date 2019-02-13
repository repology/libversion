include(GenerateExportHeader)

# sources
configure_file(config.h.in config.h @ONLY)

set(LIBVERSION_SOURCES
	compare.c
	signature.c
)

set(LIBVERSION_HEADERS
	version.h
)

# shared library
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
if(WIN32)
	# avoid clash with both c:/windows/system32/version.dll
	# and static version.lib from the next target
	set_target_properties(libversion PROPERTIES OUTPUT_NAME libversion)
endif()

# static library
add_library(libversion_static STATIC ${LIBVERSION_SOURCES} ${LIBVERSION_HEADERS})
add_dependencies(libversion_static libversion) # make sure export header is generated
target_include_directories(libversion_static PUBLIC
	$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
	$<BUILD_INTERFACE:${PROJECT_BINARY_DIR}>
	$<INSTALL_INTERFACE:include>
)
target_compile_definitions(libversion_static PUBLIC
	LIBVERSION_STATIC_DEFINE
)
set_target_properties(libversion_static PROPERTIES
	POSITION_INDEPENDENT_CODE ON
	OUTPUT_NAME version
)

# pkgconfig file
configure_file(libversion.pc.in libversion.pc @ONLY)

# installation
install(FILES
	${LIBVERSION_HEADERS}
	${CMAKE_CURRENT_BINARY_DIR}/config.h
	${CMAKE_CURRENT_BINARY_DIR}/export.h
	DESTINATION include/libversion
)
install(TARGETS libversion libversion_static EXPORT libversion LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libversion.pc DESTINATION ${PKGCONFIGDIR})

install(EXPORT libversion NAMESPACE libversion:: DESTINATION lib/cmake/libversion FILE libversionConfig.cmake)
