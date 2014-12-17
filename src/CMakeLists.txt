add_library(emdas-lib SHARED
	iset.c
	buf.c
	dh.c
	errors.c
	libemdas.c
)

set_target_properties(emdas-lib PROPERTIES
	OUTPUT_NAME "emdas"
	SOVERSION ${EMDAS_VERSION_MAJOR}
	PUBLIC_HEADER "${CMAKE_SOURCE_DIR}/include/emdas.h"
)

install(TARGETS emdas-lib
	LIBRARY DESTINATION lib
	PUBLIC_HEADER DESTINATION include
)

install(FILES
	${CMAKE_SOURCE_DIR}/include/emdas/iset.h
	${CMAKE_SOURCE_DIR}/include/emdas/buf.h
	${CMAKE_SOURCE_DIR}/include/emdas/dh.h
	${CMAKE_SOURCE_DIR}/include/emdas/errors.h
	DESTINATION include/emdas
)

add_executable(emdas
	emdas.c
)

target_link_libraries(emdas emdas-lib ${EMELF_LIBRARIES})

install(TARGETS emdas RUNTIME DESTINATION bin)

# vim: tabstop=4
