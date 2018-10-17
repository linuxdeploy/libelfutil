if(NOT TARGET gtest)
    include(AddGoogleTest)
endif()

add_subdirectory(simple_library)

add_executable(ElfFileUnitTests ElfFIleUnitTests.cpp)
add_dependencies(ElfFileUnitTests simple_library)

target_link_libraries(ElfFileUnitTests PRIVATE elfutil gtest gtest_main)
target_include_directories(ElfFileUnitTests PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_compile_definitions(ElfFileUnitTests PUBLIC SIMPLE_LIBRARY_PATH="$<TARGET_FILE:simple_library>")
set_target_properties(ElfFileUnitTests PROPERTIES BUILD_RPATH "/usr/lib")

add_gtest(ElfFileUnitTests)

