ADD_EXECUTABLE(emdas
emdas.c
image.c
iset.c
analyze.c
)

install(TARGETS emdas RUNTIME DESTINATION bin)

# vim: tabstop=4
