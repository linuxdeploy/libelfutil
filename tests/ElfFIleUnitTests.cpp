#include "gtest/gtest.h"

#include <elfutil/ElfFile.h>

TEST(ELFFileTests, testBasicFunctions) {
    boost::filesystem::path libPath(SIMPLE_LIBRARY_PATH);
    elfutil::ElfFile f(libPath);

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
    elfutil::ElfFile f(libPath);

    const auto oldRpath = f.getRPath();
    const std::string rpath = "../lib";
    f.setRPath(rpath);
    ASSERT_EQ(rpath, f.getRPath());

    f.setRPath(oldRpath);
}