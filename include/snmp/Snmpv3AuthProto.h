/**
 * @file Snmpv3AuthProto.h
 * @brief SNMPv3 Authentication protocol interface
 */

#ifndef _SNMPV3_AUTH_PROTO_H_
#define _SNMPV3_AUTH_PROTO_H_

// Includes C/C++
#include <string>
#include <memory>

// Includes 3DS
#include <3ds.h>

// Own includes
#include "Snmpv3Pdu.h"

// Defines
#define SNMPV3_AUTH_KEYLENGTH   64

namespace NetMan {

/**
 * @class Snmpv3AuthProto
 */
class Snmpv3AuthProto {
    public:
        virtual void createHash(const u8 *data, u32 length, const std::string &password, Snmpv3SecurityParams &params) = 0;
        bool authenticate(const u8 *data, u32 length, const std::string &password, Snmpv3SecurityParams &params);
        virtual std::unique_ptr<u8> passwordToKey(const std::string &password, Snmpv3SecurityParams &params) = 0;
};

}

#endif
