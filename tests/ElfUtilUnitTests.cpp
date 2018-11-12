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

TEST_F(ELFUtilTestsFixture, isElfFile) {
    ASSERT_TRUE(elfutil::isElfFile(library_path.string()));
    ASSERT_TRUE(elfutil::isElfFile(library_rpathed_path.string()));
}

TEST_F(ELFUtilTestsFixture, getSoname) {
    const auto soname = elfutil::getSoname(library_path.string());
    ASSERT_STREQ(soname.c_str(), "libsimple_library.so");
}


TEST_F(ELFUtilTestsFixture, getLinkedLibraryPathsRecursive) {
    std::set<std::string> expectedDependencies = {"libstdc++", "libc", "libm", "libgcc_s", "libsimple_library"};
    auto list = elfutil::resolveDependenciesRecursively(SIMPLE_LIBRARY2_PATH);

    std::cout << std::endl << SIMPLE_LIBRARY2_PATH << " was linked to:" << std::endl;
    for (const auto& p : list) {
        const boost::filesystem::path path = p;
        auto fileName = path.filename().stem();
        while (!fileName.extension().empty())
            fileName = fileName.stem();
        auto it = expectedDependencies.find(fileName.string());
        if (it != expectedDependencies.end())
            expectedDependencies.erase(it);

        std::cout << " - " << p << std::endl;
    }
    std::cout << std::endl;

    ASSERT_TRUE(expectedDependencies.empty());
}

TEST_F(ELFUtilTestsFixture, getLinkedLibraryPathsRecursiveWrongFile) {
    auto list = elfutil::resolveDependenciesRecursively("/dev/zero");
    ASSERT_TRUE(list.empty());
}
