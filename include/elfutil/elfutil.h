#pragma once

#include <vector>
#include <string>

#include <elfutil/errors.h>

namespace elfutil {
    /**
     * Check if <filePath> has the ELF magic bytes
     *
     * @param filePath
     * @return true if the bytes are present, false otherwise
     */
    bool isElfFile(const std::string &filePath);

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
     * Read library soname
     * @param filePath
     * @return soname if all goes ok, otherwise an empty string
     */
    std::string getSoname(const std::string& filePath);

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
     * @param filePath
     * @return libraries paths
     */
    std::vector<std::string> getLinkedLibrariesPathsRecursive(const std::string& filePath);
}
