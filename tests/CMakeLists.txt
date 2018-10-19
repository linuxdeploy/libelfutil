if(NOT TARGET gtest)
    add_subdirectory(googletest)

    # Target must already exist
    macro(add_gtest TESTNAME)
        target_link_libraries(${TESTNAME} PUBLIC gtest gmock gtest_main)
        if(GOOGLE_TEST_INDIVIDUAL)
            if(CMAKE_VERSION VERSION_LESS 3.10)
                gtest_add_tests(TARGET ${TESTNAME}
                    TEST_PREFIX "${TESTNAME}."
                    TEST_LIST TmpTestList)
                set_tests_properties(${TmpTestList} PROPERTIES FOLDER "Tests")
            else()
                gtest_discover_tests(${TESTNAME}
                    TEST_PREFIX "${TESTNAME}."
                    PROPERTIES FOLDER "Tests")
            endif()
        else()
            add_test(${TESTNAME} ${TESTNAME})
            set_target_properties(${TESTNAME} PROPERTIES FOLDER "Tests")
        endif()
    endmacro()
endif()

add_subdirectory(simple_library)

add_executable(ElfFileUnitTests ElfFIleUnitTests.cpp)
add_dependencies(ElfFileUnitTests simple_library)

target_link_libraries(ElfFileUnitTests PRIVATE elfutil gtest gtest_main)
target_include_directories(ElfFileUnitTests PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_compile_definitions(ElfFileUnitTests PUBLIC SIMPLE_LIBRARY_PATH="$<TARGET_FILE:simple_library>")
set_target_properties(ElfFileUnitTests PROPERTIES BUILD_RPATH "/usr/lib")

add_gtest(ElfFileUnitTests)
