#include "gtest/gtest.h"

#include <elfutil/elffile.h>

TEST(ELFFileTests, testBasicFunctions) {
    boost::filesystem::path libPath(SIMPLE_LIBRARY_PATH);
    elfutil::elffile f(libPath);

    printf("\n");
    printf("SystemElfABI: %d\n", f.getSystemElfABI());
    printf("SystemElfClass: %d\n", f.getSystemElfClass());
    printf("SystemElfEndianness: %d\n", f.getSystemElfEndianness());
    printf("ElfABI: %d\n", f.getElfABI());
    printf("ElfClass: %d\n", f.getElfClass());
    printf("\n");
}


TEST(ELFFileTests, setGetRpath) {
    boost::filesystem::path libPath(SIMPLE_LIBRARY_PATH);
    elfutil::elffile f(libPath);

    const auto oldRpath = f.getRPath();
    const std::string rpath = "../lib";
    f.setRPath(rpath);
    ASSERT_EQ(rpath, f.getRPath());

    f.setRPath(oldRpath);
}

TEST(ELFFileTests, traceDynamicDependencies) {
    boost::filesystem::path libPath(SIMPLE_LIBRARY_PATH);
    elfutil::elffile f(libPath);

    std::set<std::string> expectedDependencies = {"libstdc++.so", "libc.so", "libm.so", "libgcc_s.so"};
    auto list = f.traceDynamicDependencies();

    std::cout << "simple_library was linked to:" << std::endl;
    for (const auto& p : list) {
        auto it = expectedDependencies.find(p.stem().string());
        if (it != expectedDependencies.end() )
            expectedDependencies.erase(it);

        std::cout << p.stem() << std::endl;
    }

    ASSERT_TRUE(expectedDependencies.empty());
}