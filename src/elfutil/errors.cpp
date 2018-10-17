#include "elfutil/errors.h"

namespace elfutil {

    ElfFileParseError::ElfFileParseError(const std::string& msg) : std::runtime_error(msg) {}

    DependencyNotFoundError::DependencyNotFoundError(const std::string& msg) : std::runtime_error(msg) {}
}