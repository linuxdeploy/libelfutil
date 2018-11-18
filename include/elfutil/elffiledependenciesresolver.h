#pragma once

#include <vector>
#include <string>

namespace elfutil {
    class elffiledependenciesresolver {
        /**
         * Resolve the whole dependency tree of <file_path>
         * @param filePath
         * @return libraries paths
         */
        virtual std::vector<std::string> resolveDependencies(std::string elfFilePath) = 0;
    };
}
