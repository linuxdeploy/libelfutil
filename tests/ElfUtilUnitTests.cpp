#include "gtest/gtest.h"

#include <elfutil/elfutil.h>
#include <boost/filesystem.hpp>

class ELFUtilTestsFixture : public ::testing::Test {
public:
protected:
    boost::filesystem::path library_path;
    boost::filesystem::path library_rpathed_path;

    void SetUp() override {
        library_path = boost::filesystem::temp_directory_path() / boost::filesystem::unique_path("lib%%%%.so");
        boost::filesystem::copy_file(SIMPLE_LIBRARY_PATH, library_path);

        library_rpathed_path = boost::filesystem::temp_directory_path() / boost::filesystem::unique_path("lib%%%%.so");
        boost::filesystem::copy_file(SIMPLE_LIBRARY_RPATHED_PATH, library_rpathed_path);
    }

    void TearDown() override {
        boost::filesystem::remove(library_path);
        boost::filesystem::remove(library_rpathed_path);
    }
};

TEST_F(ELFUtilTestsFixture, getRpath) {
    const auto runPath = elfutil::getRunPath(library_rpathed_path.string());

    const std::string expected = "../lib";
    ASSERT_EQ(expected, expected);
}

TEST_F(ELFUtilTestsFixture, getEmptyRpath) {
    const auto runpath = elfutil::getRunPath(library_path.string());

    const std::string expected;
    ASSERT_EQ(expected, runpath);
}

TEST_F(ELFUtilTestsFixture, setRunPath) {
    const std::string expected = "../lib/x86_64";
    elfutil::setRunPath(library_path.string(), expected);

    const auto runPath = elfutil::getRunPath(library_path.string());
    ASSERT_EQ(expected, runPath);
}

TEST_F(ELFUtilTestsFixture, replaceRunPath) {
    const std::string expected = "../lib/x86_64";
    elfutil::setRunPath(library_path.string(), expected);

    const auto runPath = elfutil::getRunPath(library_path.string());
    ASSERT_EQ(expected, runPath);
}

TEST_F(ELFUtilTestsFixture, getLinkedLibrariesSonames) {
    std::set<std::string> expectedDependencies = {"libstdc++.so", "libc.so"};
    auto list = elfutil::getLinkedLibrariesSonames(library_path.string());

    std::cout << std::endl << "simple_library was linked to:" << std::endl;
    for (const auto& p : list) {
        boost::filesystem::path path = p;
        auto it = expectedDependencies.find(path.stem().string());
        if (it != expectedDependencies.end())
            expectedDependencies.erase(it);

        std::cout << p << std::endl;
    }

    ASSERT_TRUE(expectedDependencies.empty());
}

TEST_F(ELFUtilTestsFixture, getLibraryPath) {
    auto libcPath = elfutil::getLibraryPath("libc.so.6");
    std::cout << std::endl << "libc.so.6: " << libcPath << std::endl;

    ASSERT_FALSE(libcPath.empty());
}
