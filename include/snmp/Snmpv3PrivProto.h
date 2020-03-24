/**
 * @file Snmpv3PrivProto.h
 * @brief SNMPv3 Privacy protocol interface
 */

#ifndef _SNMPV3_PRIV_PROTO_H_
#define _SNMPV3_PRIV_PROTO_H_

// Includes C/C++
#include <string>
#include <memory>

// Includes 3DS
#include <3ds.h>

// Own includes
#include "Snmpv3Pdu.h"

namespace NetMan {

/**
 * @class Snmpv3PrivProto
 */
class Snmpv3PrivProto {
    public:
        virtual std::shared_ptr<BerOctetString> encrypt(const u8 *data, u32 length, const std::string &password, Snmpv3SecurityParams &params) = 0;
        virtual std::unique_ptr<u8> decrypt(std::shared_ptr<BerOctetString> data, const std::string &password, Snmpv3SecurityParams &params, u32 *length) = 0;
};

}

#endif
