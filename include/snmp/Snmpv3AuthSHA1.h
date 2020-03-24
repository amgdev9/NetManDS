/**
 * @file Snmpv3AuthSHA1.h
 * @brief SNMPv3 Authentication using SHA1
 */

#ifndef _SNMPV3_AUTH_SHA1_H_
#define _SNMPV3_AUTH_SHA1_H_

// Includes C/C++
#include <string>
#include <memory>

// Includes 3DS
#include <3ds.h>

// Own includes
#include "Snmpv3Pdu.h"
#include "Snmpv3AuthProto.h"

namespace NetMan {

/**
 * @class Snmpv3AuthSHA1
 */
class Snmpv3AuthSHA1 : public Snmpv3AuthProto {
    public:
        Snmpv3AuthSHA1();
        virtual ~Snmpv3AuthSHA1();
        void createHash(const u8 *data, u32 length, const std::string &password, Snmpv3SecurityParams &params) override;
        bool authenticate(const u8 *data, u32 length, const std::string &password, Snmpv3SecurityParams &params) override;
};

}

#endif
