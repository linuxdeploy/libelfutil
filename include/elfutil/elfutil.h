#pragma once

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
}
