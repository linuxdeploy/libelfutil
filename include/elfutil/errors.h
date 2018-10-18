#pragma once

#include <stdexcept>

namespace elfutil {
    // thrown by constructor if file is not an ELF file
    class ElfFileParseError : public std::runtime_error {
    public:
        explicit ElfFileParseError(const std::string& msg);
    };

    // thrown by traceDynamicDependencies() if a dependency is missing
    class DependencyNotFoundError : public std::runtime_error {
    public:
        explicit DependencyNotFoundError(const std::string& msg);
    };

}
