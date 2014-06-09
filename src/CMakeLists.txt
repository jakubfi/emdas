add_library(emdas SHARED
	iset.c
	libemdas.c
)

set_target_properties(emdas PROPERTIES
	SOVERSION ${EMDAS_VERSION_MAJOR}
	PUBLIC_HEADER "${CMAKE_SOURCE_DIR}/include/emdas.h"
)

install(TARGETS emdas
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
	PUBLIC_HEADER DESTINATION include
)

ADD_EXECUTABLE(emdas2
	emdas2.c
)

TARGET_LINK_LIBRARIES(emdas2 emdas)

#ADD_EXECUTABLE(emdas
#emdas.c
#image.c
#iset.c
#analyze.c
#)

#TARGET_LINK_LIBRARIES(emdas emelf)

#install(TARGETS emdas RUNTIME DESTINATION bin)

# vim: tabstop=4
