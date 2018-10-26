add_library(elfutil SHARED
    elfutil/errors.cpp
    ../include/elfutil/elfutil.h elfutil/elfutil.cpp)

target_include_directories(elfutil
    PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    PRIVATE ${LIEF_INCLUDE_DIRS}
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
)

set(elfutil_PUBLIC_HEADER
;
    ${PROJECT_SOURCE_DIR}/include/elfutil/errors.h;
)

set_target_properties(elfutil PROPERTIES
    VERSION ${libelfutil_VERSION} SOVERSION ${libelfutil_VERSION_MAJOR}
    PUBLIC_HEADER "${elfutil_PUBLIC_HEADER}"
)

target_link_libraries(elfutil PUBLIC ${Boost_LIBRARIES} ${LIB_LIEF_STATIC})

install(TARGETS elfutil
    EXPORT ElfUtilTargets
    LIBRARY DESTINATION "${INSTALL_LIB_DIR}" COMPONENT lib
    PUBLIC_HEADER DESTINATION "${INSTALL_INCLUDE_DIR}/elfutil" COMPONENT dev
)

add_dependencies(elfutil LIEF)
