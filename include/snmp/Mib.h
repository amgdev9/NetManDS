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

// Includes 3DS
#include <3ds/types.h>

namespace NetMan {

/**
 * @enum MibMacroType
 */
enum MibMacroType {
    MACRO_NONE = 0,
    MACRO_MODULE_IDENTITY,
};

/**
 * @struct MibOid
 */
typedef struct {
    std::string parentName;
    u32 value;
    std::shared_ptr<u8> macroData;
    MibMacroType macroType;
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
    public:
        Mib();
        virtual ~Mib();
};

}

#endif
