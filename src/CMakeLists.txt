add_library(emdas-lib SHARED
	iset.c
	buf.c
	dh.c
	errors.c
	libemdas.c
)

set_target_properties(emdas-lib PROPERTIES
	OUTPUT_NAME "emdas"
	SOVERSION ${APP_VERSION_MAJOR}.${APP_VERSION_MINOR}
	PUBLIC_HEADER "${CMAKE_SOURCE_DIR}/include/emdas.h"
)

include_directories(${EMAWP_INCLUDE_DIRS} ${EMELF_INCLUDE_DIRS})
target_link_libraries(emdas-lib ${EMAWP_LIBRARIES} ${EMELF_LIBRARIES})

install(TARGETS emdas-lib
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

add_executable(emdas
	emdas.c
)

target_link_libraries(emdas emdas-lib ${EMELF_LIBRARIES})

install(TARGETS emdas RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

# vim: tabstop=4
