add_library(emdas-lib SHARED
	iset.c
	libemdas.c
)

set_target_properties(emdas-lib PROPERTIES
	OUTPUT_NAME "emdas"
	SOVERSION ${EMDAS_VERSION_MAJOR}
	PUBLIC_HEADER "${CMAKE_SOURCE_DIR}/include/emdas.h"
)

install(TARGETS emdas-lib
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
	PUBLIC_HEADER DESTINATION include
)

add_executable(emdas
	emdas.c
)

target_link_libraries(emdas emdas-lib emelf)

# vim: tabstop=4
