#include <LIEF/LIEF.hpp>
#include <boost/filesystem/path.hpp>

#include <elfutil/errors.h>
#include "elfutil_private.h"

#include "LibrariesCache.h"
#include "ElfFileDependenciesResolver.h"

std::vector<std::string> splitString(const std::string& string, const char& delimiter);

using namespace std;
namespace fs = boost::filesystem;

ElfFileDependenciesResolver::ElfFileDependenciesResolver(const std::string& path, LibrariesCache* cache)
    : path(path), cache(cache) {
    try {
        elf = LIEF::ELF::Parser::parse(path);

        if (elf != nullptr)
            readRelevantElfEntries();
    } catch (const LIEF::exception& err) {
        throw elfutil::ElfFileParseError("Unable to parse: " + path);
    }
}

std::vector<std::string> ElfFileDependenciesResolver::resolveDependencies() {
    dependencies.clear();

    for (auto dependencyString: dependenciesStrings) {
        if (isAPath(dependencyString))
            processPathDependency(dependencyString);
        else {
            const auto& librarySoname = dependencyString;

            bool dependencyFound = false;
            if (mustUseRPath())
                dependencyFound = findLibraryInRPaths(librarySoname);

            if (!dependencyFound)
                dependencyFound = findLibraryInLdLibraryPaths(librarySoname);

            if (!dependencyFound)
                dependencyFound = findLibraryInRunPaths(librarySoname);

            if (!dependencyFound)
                dependencyFound = findLibraryInLdCache(librarySoname);

            if (!dependencyFound)
                dependencyFound = findLibraryInStandardPaths(librarySoname);

            if (!dependencyFound)
                missingDependencies.push_back(dependencyString);
        }
    }

    return dependencies;
}

bool ElfFileDependenciesResolver::findLibraryInRunPaths(const string& librarySoname) {
    for (const auto& path: runPath) {
        string res = cache->searchInDir(path, librarySoname);
        if (!res.empty() && hasSameArchAndEndianness(res)) {
            dependencies.push_back(res);
            return true;
        }
    }

    return false;
}

bool ElfFileDependenciesResolver::findLibraryInLdLibraryPaths(const string& librarySoname) {
    const auto ldLibraryPaths = getLdLibraryPathsEnv();

    for (const auto& path: ldLibraryPaths) {
        string res = cache->searchInDir(path, librarySoname);
        if (!res.empty() && hasSameArchAndEndianness(res)) {
            dependencies.push_back(res);
            return true;
        }

    }

    return false;
}

vector<string> ElfFileDependenciesResolver::getLdLibraryPathsEnv() const {
    char* raw = getenv("LD_LIBRARY_PATH");

    string str;
    if (raw != NULL)
        str = raw;


    if (!str.empty())
        return splitString(str, ';');

    return vector<string>();
}

bool ElfFileDependenciesResolver::findLibraryInRPaths(const string& librarySoname) {
    for (const auto& path: rPath) {
        string res = cache->searchInDir(path, librarySoname);
        if (!res.empty() && hasSameArchAndEndianness(res)) {
            dependencies.push_back(res);
            return true;
        }
    }
    return false;
}

bool ElfFileDependenciesResolver::isAPath(const string& dependencyString) const {
    return dependencyString.find("/") != dependencyString.npos;
}

bool ElfFileDependenciesResolver::mustUseRPath() const { return !rPath.empty() && runPath.empty(); }

void ElfFileDependenciesResolver::processPathDependency(const string& dependencyString) {
    boost::filesystem::path needPath(dependencyString);
    if (needPath.is_absolute())
        dependencies.push_back(dependencyString);
    else {
        auto absolute = path.parent_path() / needPath;
        dependencies.push_back(absolute.string());
    }
}

void ElfFileDependenciesResolver::readRelevantElfEntries() {
    arch = elf->header().machine_type();
    endianness = elf->header().abstract_endianness();

    readRPath();
    readRunPath();

    dependenciesStrings = readDependenciesStrings(elf);
}

void ElfFileDependenciesResolver::readRPath() {
    auto rpathStr = getDynamicEntryStringFromTag(elf, LIEF::ELF::DYNAMIC_TAGS::DT_RPATH);
    rpathStr = replaceTokens(rpathStr);
    rPath = splitString(rpathStr, ';');
}

void ElfFileDependenciesResolver::readRunPath() {
    auto runPathStr = getDynamicEntryStringFromTag(elf, LIEF::ELF::DYNAMIC_TAGS::DT_RUNPATH);
    runPathStr = replaceTokens(runPathStr);
    runPath = splitString(runPathStr, ';');
}

std::vector<std::string> splitString(const std::string& string, const char& delimiter) {
    std::vector<std::string> sections;

    istringstream f(string);
    std::string str;
    while (getline(f, str, delimiter))
        sections.push_back(str);

    return sections;
}

vector<string> ElfFileDependenciesResolver::readDependenciesStrings(unique_ptr<LIEF::ELF::Binary>& elf) const {
    vector<string> deps;
    try {
        auto dynamicSection = elf->get_section(".dynamic");
        auto dynstrSection = elf->get_section(".dynstr");

        for (const auto entry: elf->dynamic_entries()) {
            if (entry.tag() == LIEF::ELF::DYNAMIC_TAGS::DT_NEEDED) {
                const auto soname = ((char*) dynstrSection.content().data()) + entry.value();
                deps.push_back(soname);
            }
        }

    } catch (const LIEF::exception& exception) {
        cerr << exception.what();

        throw elfutil::ElfFileParseError(exception.what());
    }
    return deps;
}

std::string& ElfFileDependenciesResolver::replaceTokens(std::string& runPathStr) {
    replaceString(runPathStr, "$ORIGIN", path.parent_path().string());
    replaceString(runPathStr, "${ORIGIN}", path.parent_path().string());

    if (runPathStr.find("$LIB") != std::string::npos)
        std::cerr << "WARNING: $LIB token not suported in RunPath";

    if (runPathStr.find("${LIB}") != std::string::npos)
        std::cerr << "WARNING: ${LIB} token not suported in RunPath";

    if (runPathStr.find("$PLATFORM") != std::string::npos)
        std::cerr << "WARNING: $PLATFORM token not suported in RunPath";

    if (runPathStr.find("${PLATFORM}") != std::string::npos)
        std::cerr << "WARNING: ${PLATFORM} token not suported in RunPath";

    return runPathStr;
}

bool ElfFileDependenciesResolver::replaceString(std::string& str, const std::string& from, const std::string& to) {
    size_t start_pos = str.find(from);
    if (start_pos == std::string::npos)
        return false;

    str.replace(start_pos, from.length(), to);
    return true;
}

bool ElfFileDependenciesResolver::hasSameArchAndEndianness(const boost::filesystem::path path) {
    auto targetElf = LIEF::ELF::Parser::parse(path.string());

    bool res = true;
    res = res && arch == targetElf->header().machine_type();
    res = res && endianness == targetElf->header().abstract_endianness();
    return res;
}

bool ElfFileDependenciesResolver::findLibraryInLdCache(const std::string& soname) {
    auto candidates = cache->searchLdCache(soname);
    for (const auto& item: candidates) {
        if (hasSameArchAndEndianness(item)) {
            dependencies.push_back(item);
            return true;
        }
    }

    return false;
}

bool ElfFileDependenciesResolver::findLibraryInStandardPaths(const std::string& librarySoname) {
    static vector<string> standardPaths = {"/lib", "/usr/lib", "/lib64", "/usr/lib64"};
    for (const auto& path: standardPaths) {
        string res = cache->searchInDir(path, librarySoname);
        if (!res.empty() && hasSameArchAndEndianness(res)) {
            dependencies.push_back(res);
            return true;
        }
    }
    return false;
}

const vector<string>& ElfFileDependenciesResolver::getMissingDependencies() const {
    return missingDependencies;
}
