add_library(libelfutil SHARED elfutil/elf.cpp elfutil/log.cpp)

target_include_directories(libelfutil
    PUBLIC ${PROJECT_SOURCE_DIR}/include
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
    )

target_link_libraries(libelfutil PUBLIC ${Boost_LIBRARIES})