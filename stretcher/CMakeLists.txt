
set(SOURCES "wavstretch.c")

add_executable(stretcher ${SOURCES})
set_property(TARGET stretcher PROPERTY C_STANDARD 99)
target_link_libraries(stretcher pstretch)


