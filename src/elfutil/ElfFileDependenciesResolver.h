#pragma once

#include <string>
#include <vector>
#include <memory>

#include <LIEF/LIEF.hpp>
#include <boost/filesystem.hpp>

class LibrariesCache;
/*  From "man ld.so"

    When  resolving shared object dependencies, the dynamic linker first inspects
    each dependency string to see if it contains a slash (this can occur if a shared
    object pathname containing slashes was specified at link time).  If a slash is
    found, then the dependency string is interpreted as a (relative or absolute)
    pathname, and the shared object is loaded using that pathname.

    If a shared object dependency does not contain a slash, then it is searched for
    in the following order:

    o  Using the directories specified in the DT_RPATH dynamic section attribute of
    the binary if present and DT_RUNPATH attribute does not exist.  Use of DT_RPATH
    is deprecated.

    o  Using the environment variable LD_LIBRARY_PATH (unless the executable is
    being run in secure-execution mode; see below).  in which case it is ignored.

    o  Using the directories specified in the DT_RUNPATH dynamic section attribute
    of the binary if present.  Such directories are searched only to find those
    objects required by DT_NEEDED (direct dependencies) entries and do not apply to
    those objects' children, which must themselves have their own DT_RUNPATH
    entries.  This is unlike DT_RPATH, which is applied to searches for all children
    in the dependency tree.

    o  From the cache file /etc/ld.so.cache, which contains a compiled list of
    candidate shared objects previously found in the augmented library path.  If,
    however, the binary was linked with the -z nodeflib linker option, shared
    objects in the default paths are skipped.  Shared objects installed in hardware
    capability directories (see below) are preferred to other shared objects.

    o  In the default path /lib, and then /usr/lib.  (On some 64-bit architectures,
    the default paths for 64-bit shared objects are /lib64, and then /usr/lib64.)
    If the binary was linked with the -z nodeflib linker option,  this step is
    skipped.
 */
class ElfFileDependenciesResolver {
    boost::filesystem::path path;

    LibrariesCache *cache;
    std::unique_ptr<LIEF::ELF::Binary> elf;

    // Relevant elf entries
    LIEF::ELF::ARCH arch;
    int endianness;
    std::vector<std::string> rPath;
    std::vector<std::string> runPath;
    std::vector<std::string> dependenciesStrings;

    // results
    std::vector<std::string> dependencies;
    std::vector<std::string> missingDependencies;
public:
    ElfFileDependenciesResolver(const std::string& path, LibrariesCache* cache);

    /**
     * Resolve direct dependencies of the library at <path>
     * Use getMissingDependencies to find which dependencies were not found.
     * @return list of file paths
     */
    std::vector<std::string> resolveDependencies();

    /**
     * Return the list of dependencies that were not found after running resolveDependencies
     * @return list of libraries sonames
     */
    const std::vector<std::string>& getMissingDependencies() const;

private:
    void readRelevantElfEntries();

    std::vector<std::string> readDependenciesStrings(std::unique_ptr<LIEF::ELF::Binary>& elf) const;

    void processPathDependency(const std::string& dependencyString);

    bool mustUseRPath() const;

    std::string& replaceTokens(std::string& runPathStr);

    bool replaceString(std::string& str, const std::string& from, const std::string& to);

    void readRPath();

    void readRunPath();

    bool hasSameArchAndEndianness(const boost::filesystem::path path);

    bool isAPath(const std::string& dependencyString) const;

    bool findLibraryInRPaths(const std::string& librarySoname);

    std::vector<std::string> getLdLibraryPathsEnv() const;

    bool findLibraryInLdLibraryPaths(const std::string& librarySoname);

    bool findLibraryInRunPaths(const std::string& librarySoname);

    bool findLibraryInLdCache(const std::string& soname);

    bool findLibraryInStandardPaths(const std::string& librarySoname);
};
