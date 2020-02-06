cmake_minimum_required(VERSION 2.8.4)
project(emdas
	LANGUAGES C
)

include(GNUInstallDirs)
include(appversion.cmake)

# ---- Libraries ---------------------------------------------------------

find_package(emelf REQUIRED)
find_package(emawp REQUIRED)

# ---- Sources -----------------------------------------------------------

set(SOURCES_EMDAS_LIB
	src/iset.c
	src/buf.c
	src/dh.c
	src/errors.c
	src/libemdas.c
)

set(SOURCES_EMDAS
	src/emdas.c
)

# ---- Target: emdas-lib -------------------------------------------------

add_library(emdas-lib SHARED
	${SOURCES_EMDAS_LIB}
)

set_property(TARGET emdas-lib PROPERTY C_STANDARD 99)
target_include_directories(emdas-lib PUBLIC SYSTEM ${EMAWP_INCLUDE_DIRS})
target_include_directories(emdas-lib PUBLIC SYSTEM ${EMELF_INCLUDE_DIRS})
target_include_directories(emdas-lib PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(emdas-lib PRIVATE EMDAS_VERSION="${APP_VERSION}")
target_compile_options(emdas-lib PUBLIC -Wall)

set_target_properties(emdas-lib PROPERTIES
	OUTPUT_NAME "emdas"
	SOVERSION ${APP_VERSION_MAJOR}.${APP_VERSION_MINOR}
	PUBLIC_HEADER "${CMAKE_SOURCE_DIR}/include/emdas.h"
)

set_target_properties(emdas-lib PROPERTIES
	OUTPUT_NAME "emdas"
	SOVERSION ${APP_VERSION_MAJOR}.${APP_VERSION_MINOR}
	PUBLIC_HEADER "${CMAKE_SOURCE_DIR}/include/emdas.h"
)

target_link_libraries(emdas-lib
	${EMAWP_LIBRARIES}
	${EMELF_LIBRARIES}
)

install(TARGETS emdas-lib
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

configure_file(
	emdas-config.cmake.in
	${PROJECT_BINARY_DIR}/emdas-config.cmake
	@ONLY
)

configure_file(
	emdas-config-version.cmake.in
	${PROJECT_BINARY_DIR}/emdas-config-version.cmake
	@ONLY
)

install(FILES
	${PROJECT_BINARY_DIR}/emdas-config.cmake
	${PROJECT_BINARY_DIR}/emdas-config-version.cmake
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/emdas
	COMPONENT dev
)

# ---- Target: emdas -----------------------------------------------------

add_executable(emdas
	${SOURCES_EMDAS}
)

set_property(TARGET emdas PROPERTY C_STANDARD 99)
target_include_directories(emdas PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(emdas PRIVATE EMDAS_VERSION="${APP_VERSION}")
target_compile_options(emdas PUBLIC -Wall)

target_link_libraries(emdas
	emdas-lib
   	${EMELF_LIBRARIES}
)

install(TARGETS emdas
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# vim: tabstop=4
