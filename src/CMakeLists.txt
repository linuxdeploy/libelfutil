add_library(elfutil SHARED
    elfutil/ElfFile.cpp
    elfutil/log.cpp
    elfutil/errors.cpp)

target_include_directories(elfutil
    PUBLIC ${PROJECT_SOURCE_DIR}/include
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
    )

target_link_libraries(elfutil PUBLIC ${Boost_LIBRARIES})