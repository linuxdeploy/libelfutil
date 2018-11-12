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

std::map<std::string, LibrariesCache::LibrariesCacheMap> LibrariesCache::dirsCache;
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
    LibrariesCache::LibrariesCacheMap libEntries = getDirectoryCache(dirPath);

    auto libEntriesItr = libEntries.find(soname);
    if (libEntriesItr != libEntries.end())
        return libEntriesItr->second;

    return string();
}

LibrariesCache::LibrariesCacheMap LibrariesCache::getDirectoryCache(const string& path) {
    auto dirEntriesItr = dirsCache.find(path);
    if (dirEntriesItr == dirsCache.end())
        includeDirectoryInCache(path);

    dirEntriesItr = dirsCache.find(path);
    if (dirEntriesItr != dirsCache.end())
        return dirEntriesItr->second;
    else
        return LibrariesCache::LibrariesCacheMap();
}

void LibrariesCache::includeDirectoryInCache(const std::string dir) {
    if (boost::filesystem::is_directory(dir)) {
        LibrariesCacheMap dirCache;

        for (const auto& entry : fs::directory_iterator(dir)) {
            const auto path = entry.path().string();
            const auto filename = entry.path().filename().string();
            if (elfutil::isElfFile(path))
                dirCache.insert({filename, path});
        }

        dirsCache[dir] = dirCache;
    }
}




