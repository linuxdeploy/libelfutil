#include <LIEF/LIEF.hpp>

#include "elfutil/elfutil.h"
#include "elfutil/errors.h"

std::string getDynamicEntryStringFromTag(std::unique_ptr<LIEF::ELF::Binary>& elf, const LIEF::ELF::DYNAMIC_TAGS& tag) {
    std::string value;
    if (elf->has(tag)) {
        try {
            auto dynamicSection = elf->get_section(".dynamic");
            auto dynstrSection = elf->get_section(".dynstr");

            auto dyn = elf->get(tag);
            value = ((char*) dynstrSection.content().data()) + dyn.value();
        } catch (const LIEF::exception exception) {
            std::cerr << "Unable to read dynamic symbol";
        }
    }

    return value;
}

std::string elfutil::getRunPath(const std::string& file_path) {
    try {
        auto elf = LIEF::ELF::Parser::parse(file_path);

        std::string runpath = getDynamicEntryStringFromTag(elf, LIEF::ELF::DYNAMIC_TAGS::DT_RUNPATH);
        std::string rpath = getDynamicEntryStringFromTag(elf, LIEF::ELF::DYNAMIC_TAGS::DT_RPATH);

        if (runpath.empty())
            return rpath;
        else
            return runpath;
    } catch (const LIEF::exception& exception) {
        std::cerr << exception.what();

        throw ElfFileParseError(exception.what());
    }
}

void elfutil::setRunPath(const std::string& file_path, const std::string& runPath) {
    try {
        auto elf = LIEF::ELF::Parser::parse(file_path);
        LIEF::ELF::DynamicEntryRunPath runPathDyn(runPath);
        elf->add(runPathDyn);

        elf->write(file_path);
    } catch (const LIEF::exception& exception) {
        std::cerr << exception.what();
        throw ElfFileParseError(exception.what());
    }
}
