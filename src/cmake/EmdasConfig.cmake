find_path(EMDAS_INCLUDE_DIR emdas.h HINTS "${CMAKE_CURRENT_LIST_DIR}/../../include/")
find_library(EMDAS_LIBRARY NAMES libemdas emdas HINTS "${CMAKE_CURRENT_LIST_DIR}/../")

if(NOT EXISTS ${EMDAS_INCLUDE_DIR})
	message(FATAL_ERROR "Could not determine emdas headers location")
endif()

if(NOT EXISTS ${EMDAS_LIBRARY})
	message(FATAL_ERROR "Emdas library not found")
endif()

set(EMDAS_LIBRARIES ${EMDAS_LIBRARY})
set(EMDAS_INCLUDE_DIRS ${EMDAS_INCLUDE_DIR})

message("-- Emdas headers in: " ${EMDAS_INCLUDE_DIRS})
message("-- Emdas library: " ${EMDAS_LIBRARIES})
