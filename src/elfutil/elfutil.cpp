#include <queue>
#include <LIEF/LIEF.hpp>
#include <elfutil/elfutil.h>


#include "elfutil/elfutil.h"
#include "elfutil/errors.h"
#include "LibrariesCache.h"
#include "ElfFileDependenciesResolver.h"

using namespace std;

std::string getDynamicEntryStringFromTag(std::unique_ptr<LIEF::ELF::Binary>& elf, const LIEF::ELF::DYNAMIC_TAGS& tag) {
    std::string value;
    if (elf && elf->has(tag)) {
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

std::string elfutil::getRunPath(const std::string& filePath) {
    try {
        auto elf = LIEF::ELF::Parser::parse(filePath);

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

void elfutil::setRunPath(const std::string& filePath, const std::string& runPath) {
    try {
        auto elf = LIEF::ELF::Parser::parse(filePath);
        LIEF::ELF::DynamicEntryRunPath runPathDyn(runPath);
        elf->remove(LIEF::ELF::DYNAMIC_TAGS::DT_RUNPATH);
        elf->remove(LIEF::ELF::DYNAMIC_TAGS::DT_RPATH);

        elf->add(runPathDyn);

        elf->write(filePath);
    } catch (const LIEF::exception& exception) {
        std::cerr << exception.what();
        throw ElfFileParseError(exception.what());
    }
}

std::vector<std::string> elfutil::resolveDependenciesRecursively(const std::string& filePath) {
    LibrariesCache cache;

    set<string> visited;
    try {
        ElfFileDependenciesResolver rootFile(filePath, &cache);
        set<string> toVisit = {filePath};

        while (!toVisit.empty()) {
            const auto current = *toVisit.begin();

            ElfFileDependenciesResolver currentFile(current, &cache);

            auto directDependencies = currentFile.resolveDependencies();
            for (const auto& d: directDependencies) {
                if (visited.find(d) == visited.end() &&
                    toVisit.find(d) == toVisit.end())

                    toVisit.insert(d);
            }

            for (const auto& missing: currentFile.getMissingDependencies())
                cerr << "WARNING: " << current << " dependency not found: " << missing << endl;


            visited.insert(current);
            toVisit.erase(current);
        }

        visited.erase(filePath);

    } catch (const runtime_error& error) {
    }


    return vector<string>(visited.begin(), visited.end());
}

bool elfutil::isElfFile(const std::string& filePath) {
    try {
        std::ifstream in(filePath, std::ios::binary);
        static std::vector<char> magicNumber = {0x7f, 'E', 'L', 'F'};
        auto itr = std::istreambuf_iterator<char>(in);

        for (int i = 0; i < 4; i++, itr++)
            if (*itr != magicNumber[i])
                return false;

    } catch (const std::runtime_error& err) {
        return false;
    }

    return true;
}

std::string elfutil::getSoname(const std::string& filePath) {
    try {
        auto elf = LIEF::ELF::Parser::parse(filePath);
        std::string soname = getDynamicEntryStringFromTag(elf, LIEF::ELF::DYNAMIC_TAGS::DT_SONAME);

        return soname;
    } catch (const LIEF::exception& exception) {
        std::cerr << exception.what();

        return std::string();
    }
}
