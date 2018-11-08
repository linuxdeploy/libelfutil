#pragma once

#include <vector>
#include <string>

#include <elfutil/errors.h>

namespace elfutil {
    /**
     * Read the elf file RUNPATH. Fallbacks to RPATH if the RUNPATH entry is not found.
     * @param file_path
     * @return RUNPATH string
     * @throw ElfFileParseError in case of error
     */
    std::string getRunPath(const std::string& file_path);

    /**
     * Replace or insert the RUNPATH entry in the <file_path>
     * @param file_path
     * @param runPath
     * @return true on success, false otherwise
     * @throw ElfFileParseError in case of error
     */
    void setRunPath(const std::string& file_path, const std::string& runPath);


    /**
     * Read the linked libraries sonames.
     *
     * @param file_path
     * @return linked libraries sonames
     */
    std::vector<std::string> getLinkedLibrariesSonames(const std::string& file_path);


    /**
     * Resolve the location in the system of the a library with <soname>.
     * Uses ld.so.cache data to resolve the real file names.
     *
     * @param soname
     * @return libary file path
     */
    std::string getLibraryPath(const std::string& soname);

    /**
     * Resolve the whole dependency tree of <file_path>
     * @param file_path
     * @return libraries paths
     */
    std::string getLinkedLibraryPathsRecursive(const std::string& file_path);
}
