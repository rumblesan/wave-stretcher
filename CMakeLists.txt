# much of this pulled from the cmake wiki
# https://cmake.org/Wiki/CMake/Tutorials/How_to_create_a_ProjectConfig.cmake_file
cmake_minimum_required(VERSION 3.0)
project(libpstretch C)

set(LIBPSTRETCH_MAJOR_VERSION 1)
set(LIBPSTRETCH_MINOR_VERSION 0)
set(LIBPSTRETCH_PATCH_VERSION 0)
set(LIBPSTRETCH_VERSION
    ${LIBPSTRETCH_MAJOR_VERSION}.${LIBPSTRETCH_MINOR_VERSION}.${LIBPSTRETCH_PATCH_VERSION})

# Offer the user the choice of overriding the installation directories
set(INSTALL_LIB_DIR lib
    CACHE PATH "Installation directory for libraries")
set(INSTALL_BIN_DIR bin
    CACHE PATH "Installation directory for executables")
set(INSTALL_INCLUDE_DIR include
    CACHE PATH "Installation directory for header files")

if(WIN32 AND NOT CYGWIN)
    set(DEF_INSTALL_CMAKE_DIR cmake)
else()
    set(DEF_INSTALL_CMAKE_DIR lib/cmake/LibPStretch)
endif()
set(INSTALL_CMAKE_DIR
    ${DEF_INSTALL_CMAKE_DIR} CACHE PATH
    "Installation directory for CMake files")

# Make relative paths absolute (needed later on)
foreach(p LIB BIN INCLUDE CMAKE)
    set(var INSTALL_${p}_DIR)
    if(NOT IS_ABSOLUTE "${${var}}")
        set(${var} "${CMAKE_INSTALL_PREFIX}/${${var}}")
    endif()
endforeach()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")


add_subdirectory(lib)
add_subdirectory(stretcher)

# Add all targets to the build-tree export set
export(TARGETS pstretch
    FILE "${PROJECT_BINARY_DIR}/LibPStretchTargets.cmake")

# Export the package for use from the build-tree
# (this registers the build-tree with a global CMake-registry)
export(PACKAGE LibPStretch)

# Create the LibPStretchConfig.cmake and LibPStretchConfigVersion files
file(RELATIVE_PATH REL_INCLUDE_DIR "${INSTALL_CMAKE_DIR}"
    "${INSTALL_INCLUDE_DIR}")
# ... for the build tree
set(CONF_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}" "${PROJECT_BINARY_DIR}")
configure_file(LibPStretchConfig.cmake.in
    "${PROJECT_BINARY_DIR}/LibPStretchConfig.cmake" @ONLY)
# ... for the install tree
set(CONF_INCLUDE_DIRS "\${INSTALL_CMAKE_DIR}/${REL_INCLUDE_DIR}")
configure_file(LibPStretchConfig.cmake.in
    "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/LibPStretchConfig.cmake" @ONLY)
# ... for both
configure_file(LibPStretchConfigVersion.cmake.in
    "${PROJECT_BINARY_DIR}/LibPStretchConfigVersion.cmake" @ONLY)

# Install the LibPStretchConfig.cmake and LibPStretchConfigVersion.cmake
install(FILES
    "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/LibPStretchConfig.cmake"
    "${PROJECT_BINARY_DIR}/LibPStretchConfigVersion.cmake"
    DESTINATION "${INSTALL_CMAKE_DIR}" COMPONENT dev)

# Install the export set for use with the install-tree
install(EXPORT LibPStretchTargets DESTINATION
    "${INSTALL_CMAKE_DIR}" COMPONENT dev)

