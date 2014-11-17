cmake_minimum_required(VERSION 2.8.4)
project(emdas C)

set(DEBUG 0)

set(EMDAS_VERSION_MAJOR 2)
set(EMDAS_VERSION_MINOR 1)
set(EMDAS_VERSION_PATCH 1)
set(EMDAS_VERSION ${EMDAS_VERSION_MAJOR}.${EMDAS_VERSION_MINOR}.${EMDAS_VERSION_PATCH})

include_directories(${CMAKE_SOURCE_DIR}/include)

add_definitions(-Wall -std=c99 -D_XOPEN_SOURCE=700 -DEMDAS_VERSION="${EMDAS_VERSION}")

if(DEBUG)
	add_definitions(-DDEBUG -g -O0)
else(DEBUG)
	add_definitions(-DNDEBUG -O3)
endif(DEBUG)

add_subdirectory(src build)

# vim: tabstop=4
