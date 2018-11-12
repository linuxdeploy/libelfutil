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
    bool isElfFile(const std::string& filePath);

    /**
     * Read the elf file RUNPATH. Fallbacks to RPATH if the RUNPATH entry is not found.
     * @param filePath
     * @return RUNPATH string
     * @throw ElfFileParseError in case of error
     */
    std::string getRunPath(const std::string& filePath);

    /**
     * Replace or insert the RUNPATH entry in the <file_path>
     * @param filePath
     * @param runPath
     * @return true on success, false otherwise
     * @throw ElfFileParseError in case of error
     */
    void setRunPath(const std::string& filePath, const std::string& runPath);
    
    /**
     * Read library soname
     * @param filePath
     * @return soname if all goes ok, otherwise an empty string
     */
    std::string getSoname(const std::string& filePath);


    /**
     * Resolve the whole dependency tree of <file_path>
     * @param filePath
     * @return libraries paths
     */
    std::vector<std::string> resolveDependenciesRecursively(const std::string& filePath);
}
