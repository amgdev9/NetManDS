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

// Defines
#define MIBLOADER_TOKEN_SIZE    48
#define MIBLOADER_STR_SIZE      65535

namespace NetMan {

/**
 * @class MibLoader
 */
class MibLoader {
    private:
        void skipUntilReadable(FILE *f);
        void getToken(FILE *f, char *token);
        void checkToken(FILE *f, u32 compToken);
        bool compareToken(char *token, const char *compToken);
        void getQuotedString(FILE *f, char *str);
        void decodeOID(FILE *f, char *token, MibOid *oid);
    public:
        MibLoader();
        std::shared_ptr<Mib> load(const std::string &path);
        virtual ~MibLoader();
};

}

#endif
