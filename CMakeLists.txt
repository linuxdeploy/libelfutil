cmake_minimum_required(VERSION 3.8)
project(libelfutil
    VERSION 0.0.1
    LANGUAGES C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Offer the user the choice of overriding the installation directories
set(INSTALL_LIB_DIR lib CACHE PATH "Installation directory for libraries")
set(INSTALL_BIN_DIR bin CACHE PATH "Installation directory for executables")
set(INSTALL_INCLUDE_DIR include CACHE PATH "Installation directory for header files")
set(INSTALL_CMAKE_DIR lib/cmake/ElfUtil CACHE PATH "Installation directory for CMake files")


set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake/modules/")

find_package(Boost COMPONENTS filesystem regex REQUIRED)

include(FetchLIEF)
add_subdirectory(src)

option(BUILD_TEST "Build the tests" ON)
if(BUILD_TEST)
    enable_testing()
    add_subdirectory(tests)
endif()

include(ExportElfUtilTargets)
include(FeatureSummary)
feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
