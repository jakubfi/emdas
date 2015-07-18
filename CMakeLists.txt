cmake_minimum_required(VERSION 2.8.4)
project(emdas C)

find_package(emelf REQUIRED)
find_package(emawp REQUIRED)

set(DEBUG 0)

set(EMDAS_VERSION_MAJOR 2)
set(EMDAS_VERSION_MINOR 2)
set(EMDAS_VERSION_PATCH 0)
set(EMDAS_VERSION ${EMDAS_VERSION_MAJOR}.${EMDAS_VERSION_MINOR}.${EMDAS_VERSION_PATCH})

include_directories(${CMAKE_SOURCE_DIR}/include)

add_definitions(-Wall -std=c99 -D_XOPEN_SOURCE=700 -DEMDAS_VERSION="${EMDAS_VERSION}")

if(DEBUG)
	add_definitions(-DDEBUG -g -O0)
else(DEBUG)
	add_definitions(-DNDEBUG -O3)
endif(DEBUG)

add_subdirectory(src)

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
	DESTINATION lib/emdas
	COMPONENT dev
)

# vim: tabstop=4
