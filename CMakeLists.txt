cmake_minimum_required(VERSION 2.8.4)
project(emdas C)

set(EMDAS_VERSION_MAJOR 0)
set(EMDAS_VERSION_MINOR 2)
set(EMDAS_VERSION_PATCH 0)
set(EMDAS_VERSION ${EMDAS_VERSION_MAJOR}.${EMDAS_VERSION_MINOR}.${EMDAS_VERSION_PATCH})

include_directories(${CMAKE_SOURCE_DIR}/include)

add_definitions(-Wall -std=c99 -D_XOPEN_SOURCE=700 -O2 -DEMDAS_VERSION="${EMDAS_VERSION}" -g)

add_subdirectory(src build)

# vim: tabstop=4
