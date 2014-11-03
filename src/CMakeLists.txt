add_library(emdas-lib SHARED
	iset.c
	buf.c
	dh.c
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

install(FILES
	../include/emdas/iset.h
	../include/emdas/buf.h
	../include/emdas/dh.h
	DESTINATION include/emdas
)

add_executable(emdas
	emdas.c
)

target_link_libraries(emdas emdas-lib emelf)

install(TARGETS emdas RUNTIME DESTINATION bin)

# vim: tabstop=4
