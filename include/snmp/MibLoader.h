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
        char skipUntilReadable(FILE *f);
        void getToken(FILE *f, char *token);
        void checkToken(FILE *f, u32 compToken);
        inline bool compareToken(char *token, const char *compToken) { return (strcmp(token, compToken) == 0); }
        void getQuotedString(FILE *f, char *str, char start = '\"', char end = '\"');
        void decodeOID(FILE *f, char *token, MibOid *oid);
        std::shared_ptr<MibOid> addOid(std::unordered_map<std::string, std::shared_ptr<MibOid>> &oidMap, const std::string &name, std::shared_ptr<MibOid> oid, MibMacroType macroType, std::shared_ptr<u8> macroData);
        std::shared_ptr<Mib> smiMib;
        MibLoader();
        virtual ~MibLoader();
    public:
        void loadSMI(const std::string &path);
        static MibLoader &getInstance();
        std::shared_ptr<Mib> load(const std::string &path);
};

}

#endif
