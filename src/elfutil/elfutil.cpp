#include <LIEF/LIEF.hpp>
#include <elfutil/elfutil.h>


#include "elfutil/elfutil.h"
#include "elfutil/errors.h"
#include "DependenciesMapper.h"

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

std::vector<std::string> elfutil::getLinkedLibrariesSonames(const std::string& file_path) {
    std::vector<std::string> sonames;
    try {
        auto elf = LIEF::ELF::Parser::parse(file_path);
        auto dynamicSection = elf->get_section(".dynamic");
        auto dynstrSection = elf->get_section(".dynstr");

        for (const auto entry: elf->dynamic_entries()) {
            if (entry.tag() == LIEF::ELF::DYNAMIC_TAGS::DT_NEEDED) {
                const auto soname = ((char*) dynstrSection.content().data()) + entry.value();
                sonames.push_back(soname);
            }
        }
    } catch (const LIEF::exception& exception) {
        std::cerr << exception.what();

        throw ElfFileParseError(exception.what());
    }
    return sonames;
}

std::string elfutil::getLibraryPath(const std::string& soname) {
    DependenciesMapper mapper;
    return mapper.getLibaryPath(soname);
}

std::vector<std::string> elfutil::getLinkedLibrariesPathsRecursive(const std::string& filePath) {
    DependenciesMapper mapper;
    return mapper.listDependenciesRecursive(filePath);
}

bool elfutil::isElfFile(const std::string& filePath) {
    std::ifstream in(filePath, std::ios::binary);

    static std::vector<char> magicNumber = {0x7f, 'E', 'L', 'F'};
    auto itr = std::istreambuf_iterator<char>(in);
    for (int i = 0; i < 4; i++, itr++)
        if (*itr != magicNumber[i])
            return false;

    return true;
}
