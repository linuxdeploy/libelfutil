#pragma once

#include <map>
#include <string>
#include <vector>

class DependenciesMapper {
    static std::map<std::string, std::string> map;

public:
    std::string getLibaryPath(const std::string& soname);

    std::vector<std::string> listDependencies(const std::string& path);

protected:
    static void readLdCache();
};
