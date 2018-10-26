add_library(simple_library SHARED simple_library.cpp)

set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
set(CMAKE_INSTALL_RPATH "../lib")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

add_library(simple_library_rpathed SHARED simple_library.cpp)
