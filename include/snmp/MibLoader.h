/**
 * @file MibLoader.h
 * @brief MIB loader
 */
#ifndef _MIBLOADER_H_
#define _MIBLOADER_H_

// Includes C/C++
#include <memory>

// Own includes
#include "snmp/Mib.h"

namespace NetMan {

/**
 * @class MibLoader
 */
class MibLoader {
    private:
        void skipUntilReadable(FILE *f);
    public:
        MibLoader();
        std::shared_ptr<Mib> load(const std::string &path);
        virtual ~MibLoader();
};

}

#endif
