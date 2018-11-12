#pragma once

#include <map>
#include <set>
#include <string>
#include <vector>

#include <LIEF/LIEF.hpp>

#include <elfutil/errors.h>

class LibrariesCache {

    // Soname -> Path map
    typedef std::multimap<std::string, std::string> LibrariesCacheMap;

    static LibrariesCacheMap ldCache;
    static std::map<std::string, LibrariesCacheMap> dirsCache;

public:
    LibrariesCache();

    std::vector<std::string> searchLdCache(const std::string soname);

    std::string searchInDir(const std::string dirPath, const std::string soname);

protected:
    static void readLdCache();

    void includeDirectoryInCache(const std::string dir);

    LibrariesCacheMap getDirectoryCache(const std::string& path);
};
