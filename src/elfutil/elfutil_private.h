#pragma once

#include <LIEF/ELF/Binary.hpp>

#include "elfutil/elfutil.h"

std::string getDynamicEntryStringFromTag(std::unique_ptr<LIEF::ELF::Binary>& elf, const LIEF::ELF::DYNAMIC_TAGS& tag);
