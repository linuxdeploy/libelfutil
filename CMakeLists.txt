cmake_minimum_required(VERSION 3.8)
project(libelfutil C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake/modules/")

find_package(Boost COMPONENTS program_options filesystem regex REQUIRED)

add_subdirectory(src)

option(BUILD_TEST "Build the tests" ON)
if(BUILD_TEST)
    enable_testing()
    add_subdirectory(tests)
endif()


include(FeatureSummary)
feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
