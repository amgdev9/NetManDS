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

namespace NetMan {

/**
 * @class Snmpv3AuthProto
 */
class Snmpv3AuthProto {
    public:
        virtual void createHash(std::unique_ptr<u8> data, const std::string &password, Snmpv3SecurityParams &params) = 0;
        virtual bool authenticate(std::unique_ptr<u8> data, const std::string &password, Snmpv3SecurityParams &params) = 0;
};

}

#endif
