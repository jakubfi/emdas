cmake_minimum_required(VERSION 3.0)
project(emdas
	DESCRIPTION "MERA-400 binaries disassembler"
	LANGUAGES C
)

include(GNUInstallDirs)
include(cmake/appversion.cmake)

include_directories(${CMAKE_SOURCE_DIR}/include)
add_compile_options(-Wall)

# ---- Target: emdas-lib -------------------------------------------------

find_package(emawp 3.0 REQUIRED)

set(EMDAS_PUBLIC_HEADERS
	include/emdas.h
)

add_library(emdas-lib SHARED
	src/iset.c
	src/iset.h
	src/buf.c
	src/buf.h
	src/dh.c
	src/dh.h
	src/errors.c
	src/libemdas.c
	src/opfields.h
	${EMDAS_PUBLIC_HEADERS}
)

target_link_libraries(emdas-lib
	emawp
)

set_target_properties(emdas-lib PROPERTIES
	C_STANDARD 99
	OUTPUT_NAME "emdas"
	SOVERSION ${APP_VERSION_MAJOR}.${APP_VERSION_MINOR}
	PUBLIC_HEADER "${EMDAS_PUBLIC_HEADERS}"
)

target_compile_definitions(emdas-lib PRIVATE EMDAS_VERSION="${APP_VERSION}")

install(TARGETS emdas-lib
	EXPORT emdasLibTarget
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

configure_file(
	cmake/emdas-config.cmake.in
	${PROJECT_BINARY_DIR}/emdas-config.cmake
	@ONLY
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
	"${CMAKE_CURRENT_BINARY_DIR}/emdas-config-version.cmake"
	VERSION ${APP_VERSION}
	COMPATIBILITY AnyNewerVersion
)

export(
	EXPORT emdasLibTarget
	FILE "${CMAKE_CURRENT_BINARY_DIR}/emdas-config-targets.cmake"
)

set(ConfigPackageLocation ${CMAKE_INSTALL_LIBDIR}/cmake/emdas)
configure_package_config_file(cmake/emdas-config.cmake.in
	"${CMAKE_CURRENT_BINARY_DIR}/emdas-config.cmake"
	INSTALL_DESTINATION ${ConfigPackageLocation}
	NO_SET_AND_CHECK_MACRO
	NO_CHECK_REQUIRED_COMPONENTS_MACRO
)

install(EXPORT emdasLibTarget
	FILE emdas-config-targets.cmake
	DESTINATION ${ConfigPackageLocation}
)

install(FILES
	"${CMAKE_CURRENT_BINARY_DIR}/emdas-config.cmake"
	"${CMAKE_CURRENT_BINARY_DIR}/emdas-config-version.cmake"
	DESTINATION ${ConfigPackageLocation}
	COMPONENT Devel
)

# ---- Target: emdas -----------------------------------------------------

add_executable(emdas
	src/emdas.c
)

target_link_libraries(emdas emdas-lib)

set_property(TARGET emdas PROPERTY C_STANDARD 99)
target_compile_definitions(emdas PRIVATE EMDAS_VERSION="${APP_VERSION}")
install(TARGETS emdas RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

# vim: tabstop=4
