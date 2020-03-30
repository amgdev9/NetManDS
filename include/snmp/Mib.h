/**
 * @file Mib.h
 * @brief MIB holder
 */
#ifndef _MIB_H_
#define _MIB_H_

// Includes C/C++
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

// Includes 3DS
#include <3ds/types.h>

// Own includes
#include "asn1/BerOid.h"

namespace NetMan {

/**
 * @enum MibMacroType
 */
enum MibMacroType {
    MACRO_NONE = 0,
    MACRO_MODULE_IDENTITY,
    MACRO_OBJECT_TYPE,
};

/**
 * @struct MibObjectType
 */
typedef struct {
    std::string syntax;
    std::string units;
    std::string maxAccess;
    std::string status;
    std::string description;
    std::string reference;
    std::string indexPart;
    std::string defaultValue;
} MibObjectType;

/**
 * @struct MibOid
 */
typedef struct MibOid_t {
    std::string parentName;
    u32 value;
    std::shared_ptr<u8> macroData;
    MibMacroType macroType;
    std::unordered_map<std::string, std::shared_ptr<struct MibOid_t>> children;
} MibOid;

/**
 * @struct MibRevision
 */
typedef struct {
    std::string date;
    std::string description;
} MibRevision;

/**
 * @struct MibModuleIdentity
 */
typedef struct {
    std::string lastUpdated;
    std::string organization;
    std::string contactInfo;
    std::string description;
    std::vector<MibRevision> revisions;
} MibModuleIdentity;

/**
 * @class Mib
 */
class Mib {
    private:
        std::shared_ptr<std::unordered_map<std::string, std::shared_ptr<MibOid>>> oidTree;
    public:
        Mib(std::shared_ptr<std::unordered_map<std::string, std::shared_ptr<MibOid>>> tree);
        std::shared_ptr<BerOid> resolve(const std::string &name);
        void print();
        inline std::shared_ptr<std::unordered_map<std::string, std::shared_ptr<MibOid>>> getOidTree() { return oidTree; }
        virtual ~Mib();
};

}

#endif
