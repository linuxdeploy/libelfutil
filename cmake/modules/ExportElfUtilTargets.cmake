export(TARGETS elfutil
    FILE "${PROJECT_BINARY_DIR}/ElfUtilTargets.cmake")

# Export the package for use from the build-tree
# (this registers the build-tree with a global CMake-registry)
export(PACKAGE ElfUtil)

# Make relative paths absolute (needed later on)
foreach(p LIB BIN INCLUDE CMAKE)
    set(var INSTALL_${p}_DIR)
    if(NOT IS_ABSOLUTE "${${var}}")
        set(${var} "${CMAKE_INSTALL_PREFIX}/${${var}}")
    endif()
endforeach()

# Create the ElfUtilConfig.cmake and ElfUtilConfigVersion files
file(RELATIVE_PATH REL_INCLUDE_DIR "${INSTALL_CMAKE_DIR}"
    "${INSTALL_INCLUDE_DIR}")
# ... for the build tree
set(CONF_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/include" "${PROJECT_BINARY_DIR}/include")
configure_file(cmake/package/ElfUtilConfig.cmake.in
    "${PROJECT_BINARY_DIR}/ElfUtilConfig.cmake" @ONLY)
# ... for the install tree
set(CONF_INCLUDE_DIRS "\${ELFUTIL_CMAKE_DIR}/${REL_INCLUDE_DIR}")
configure_file(cmake/package/ElfUtilConfig.cmake.in
    "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/ElfUtilConfig.cmake" @ONLY)
# ... for both
configure_file(cmake/package/ElfUtilConfigVersion.cmake.in
    "${PROJECT_BINARY_DIR}/ElfUtilConfigVersion.cmake" @ONLY)

# Install the ElfUtilConfig.cmake and ElfUtilConfigVersion.cmake
install(FILES
    "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/ElfUtilConfig.cmake"
    "${PROJECT_BINARY_DIR}/ElfUtilConfigVersion.cmake"
    DESTINATION "${INSTALL_CMAKE_DIR}" COMPONENT dev)

# Install the export set for use with the install-tree
install(EXPORT ElfUtilTargets DESTINATION
    "${INSTALL_CMAKE_DIR}" COMPONENT dev)