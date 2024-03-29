include(GenerateExportHeader)

# sources
configure_file(config.h.in config.h @ONLY)

set(LIBVERSION_SOURCES
	private/compare.c
	private/parse.c
	compare.c
)

set(LIBVERSION_HEADERS
	version.h
)

set(LIBVERSION_PRIVATE_HEADERS
	private/compare.h
	private/component.h
	private/parse.h
	private/string.h
)

# shared library
add_library(libversion SHARED ${LIBVERSION_SOURCES} ${LIBVERSION_HEADERS} ${LIBVERSION_PRIVATE_HEADERS})
target_include_directories(libversion PUBLIC
	$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
	$<BUILD_INTERFACE:${PROJECT_BINARY_DIR}>
	$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
set_target_properties(libversion PROPERTIES
	SOVERSION 1
	VERSION 1.0.0
	OUTPUT_NAME version
	C_VISIBILITY_PRESET hidden
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
	$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
target_compile_definitions(libversion_static PUBLIC
	LIBVERSION_STATIC_DEFINE
)
set_target_properties(libversion_static PROPERTIES
	POSITION_INDEPENDENT_CODE ON
	OUTPUT_NAME version
)

# object library
add_library(libversion_object OBJECT EXCLUDE_FROM_ALL ${LIBVERSION_SOURCES} ${LIBVERSION_HEADERS})
add_dependencies(libversion_object libversion) # make sure export header is generated
target_include_directories(libversion_object PUBLIC
	$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
	$<BUILD_INTERFACE:${PROJECT_BINARY_DIR}>
)
target_compile_definitions(libversion_object PUBLIC
	LIBVERSION_STATIC_DEFINE
)

# pkgconfig file
if(IS_ABSOLUTE "${CMAKE_INSTALL_LIBDIR}")
	set(libdir_for_pc_file "${CMAKE_INSTALL_LIBDIR}")
else()
	set(libdir_for_pc_file "\${exec_prefix}/${CMAKE_INSTALL_LIBDIR}")
endif()

if(IS_ABSOLUTE "${CMAKE_INSTALL_INCLUDEDIR}")
	set(includedir_for_pc_file "${CMAKE_INSTALL_INCLUDEDIR}")
else()
	set(includedir_for_pc_file "\${prefix}/${CMAKE_INSTALL_INCLUDEDIR}")
endif()

configure_file(libversion.pc.in libversion.pc @ONLY)

# installation
install(FILES
	${LIBVERSION_HEADERS}
	${CMAKE_CURRENT_BINARY_DIR}/config.h
	${CMAKE_CURRENT_BINARY_DIR}/export.h
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/libversion
)
install(TARGETS libversion libversion_static EXPORT libversion)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libversion.pc DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)

install(EXPORT libversion NAMESPACE libversion:: DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/libversion FILE libversionConfig.cmake)
