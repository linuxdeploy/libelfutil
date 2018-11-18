#pragma once

#include <vector>
#include <string>

#include <elfutil/errors.h>
#include <elfutil/elffile.h>
#include <elfutil/elffiledependenciesresolver.h>
#include <boost/shared_ptr.hpp>

namespace elfutil {
    typedef enum {
        PATCHELF_READELF_APPS,
        LIEF
    } ElfFileImplementations;

    typedef enum {
        LDD_APP,
        LIEF_AND_LD_CACHE
    } ElfFileDependenciesResolverImplementations;

    /**
     * Create an instance of elffile using the requested <implementation>
     * @param elfFilePath
     * @param implementation
     * @return elffile instance
     */
    std::shared_ptr<elffile> openElfFile(std::string elfFilePath,
                                         ElfFileImplementations implementation = PATCHELF_READELF_APPS);

    /**
     * Create an instance of the ElfFileDependenciesResolver using the requested <implementation>
     * @param elfFilePath
     * @param implementation
     * @return instance of the ElfFileDependenciesResolver
     */
    std::shared_ptr<elffiledependenciesresolver> createElfFileDependenciesResolver(std::string elfFilePath,
                                                                                   ElfFileDependenciesResolverImplementations implementation = LDD_APP);
}
