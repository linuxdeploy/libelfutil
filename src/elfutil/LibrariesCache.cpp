#include <iostream>
#include <boost/filesystem.hpp>
#include <LIEF/LIEF.hpp>

#include "ldcache.hpp"
#include "LibrariesCache.h"
#include "elfutil/elfutil.h"
#include "elfutil/errors.h"

#include "elfutil_private.h"

using namespace std;
namespace fs = boost::filesystem;

LibrariesCache::LibrariesCacheMap LibrariesCache::ldCache;

LibrariesCache::LibrariesCache() {
    readLdCache();
}


void LibrariesCache::readLdCache() {
    try {
        auto list = ldcache::parse("/etc/ld.so.cache");
        for (const auto& entry: list)
            ldCache.insert({entry.name, {entry.path}});
    } catch (const ldcache::Error& error) {
        cerr << error.what() << endl;
    }
}

std::vector<std::string> LibrariesCache::searchLdCache(const std::string soname) {
    const auto range = ldCache.equal_range(soname); // find all entries with key <soname>
    if (range.first->first == soname) { // was found ?
        vector<string> items;
        for (auto rItr = range.first; rItr != range.second; ++rItr)
            items.push_back(rItr->second);

        return items;
    }
    return vector<string>();
}

std::string LibrariesCache::searchInDir(const std::string dirPath, const std::string soname) {
    if (boost::filesystem::exists(dirPath + "/" + soname)) {
        return dirPath + "/" + soname;
    } else
        return string();
}




