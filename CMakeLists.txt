CMAKE_MINIMUM_REQUIRED(VERSION 2.8.4)
PROJECT(emdas C)

set(EMAS_VERSION 1.0.0)

ADD_DEFINITIONS(-Wall -std=c99 -D_XOPEN_SOURCE=700 -O2 -DEMDAS_VERSION="${EMDAS_VERSION}" -g)

include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_BINARY_DIR})

ADD_EXECUTABLE(emdas
emdas.c
image.c
iset.c
analyze.c
)

install(TARGETS emdas RUNTIME DESTINATION bin)

# vim: tabstop=4
