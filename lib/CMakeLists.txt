
include_directories(include)

set(HEADERS
  "include/fft.h"
  "include/mp3file.h"
  "include/pstretch.h"
  "include/sample.h"
  "include/stretch.h"
  "include/wavefile.h"
  )

set(SOURCES
  "src/fft.c"
  "src/mp3file.c"
  "src/sample.c"
  "src/stretch.c"
  "src/wavefile.c"
  )

add_library(pstretch STATIC ${SOURCES})

set_target_properties(pstretch PROPERTIES
    PUBLIC_HEADER "${HEADERS}")

install(TARGETS pstretch
    DESTINATION lib
    EXPORT LibPStretchTargets
    LIBRARY DESTINATION "${INSTALL_LIB_DIR}" COMPONENT shlib
    PUBLIC_HEADER DESTINATION "${INSTALL_INCLUDE_DIR}/pstretch"
    COMPONENT dev
    )
target_link_libraries(pstretch fftw3f sndfile mad mp3lame)
