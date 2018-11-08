#include <iostream>
#include <LIEF/LIEF.hpp>

#include "ldcache.hpp"
#include "DependenciesMapper.h"
#include "elfutil_private.h"

using namespace std;

std::map<std::string, std::string> DependenciesMapper::map;

void DependenciesMapper::readLdCache() {
    try {
        auto list = ldcache::parse("/etc/ld.so.cache");
        for (const auto& entry: list)
            map[entry.name] = entry.path;
    } catch (const ldcache::Error& error) {
        cerr << error.what() << endl;
    }
}

std::vector<std::string> DependenciesMapper::listDependencies(const std::string& path) {
    if (map.empty())
        readLdCache();

    vector<string> sonames;
    try {
        auto elf = LIEF::ELF::Parser::parse(path);

        auto dynamicSection = elf->get_section(".dynamic");
        auto dynstrSection = elf->get_section(".dynstr");
        for (const auto entry: elf->dynamic_entries()) {
            if (entry.tag() == LIEF::ELF::DYNAMIC_TAGS::DT_NEEDED) {
                string soname = ((char*) dynstrSection.content().data()) + entry.value();
                sonames.push_back(soname);
            }
        }

    } catch (const LIEF::exception& exception) {
        std::cerr << exception.what();
    }


    for (const auto &soname: sonames) {

    }
    return std::vector<std::string>();
}

std::string DependenciesMapper::getLibaryPath(const std::string& soname) {
    if (map.empty())
        readLdCache();
    const auto itr = map.find(soname);
    if (itr != map.end())
        return itr->second;

    return std::string();
}
