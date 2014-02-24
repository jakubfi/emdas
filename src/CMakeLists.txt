ADD_EXECUTABLE(emdas
emdas.c
image.c
iset.c
analyze.c
)

TARGET_LINK_LIBRARIES(emdas emelf)

install(TARGETS emdas RUNTIME DESTINATION bin)

# vim: tabstop=4
