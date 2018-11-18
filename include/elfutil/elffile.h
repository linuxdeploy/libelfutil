#pragma once

#include <elfutil/errors.h>

#include <string>
#include <vector>

namespace elfutil {

    class elffile {
    public:

        /**
         * Read the machine type code
         * @return arch code as in the Elf header
         */
        virtual unsigned int getArchCode() = 0;

        /**
         * @return endianness code as in the Elf header
         */
        virtual unsigned int getEndiannessCode() = 0;

        /**
         * Replace RPath and RunPath entries by the value at <runPath>
         * @param runPath
         */
        virtual void setRunPath(const std::string& runPath) = 0;

        /**
         * Reads value pointed by the RunPath dynamic entry. If RunPath entry is not found the RPath value will be used
         * @return RunPath entry value
         */
        virtual std::string getRunPath() = 0;

        /**
         * Read the values pointed by the DT_NEEDED entries in the dynamic section of the Elf file.
         * @return DT_NEEDED entries values
         */
        virtual std::vector<std::string> getDependencyStrings() = 0;
    };

}
