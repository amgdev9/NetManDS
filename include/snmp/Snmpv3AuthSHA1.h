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

namespace NetMan {

/**
 * @class Snmpv3AuthSHA1
 */
class Snmpv3AuthSHA1 {
    public:
        Snmpv3AuthSHA1();
        virtual ~Snmpv3AuthSHA1();
        void createHash(std::unique_ptr<u8> data, const std::string &password, Snmpv3SecurityParams &params);
        bool authenticate(std::unique_ptr<u8> data, const std::string &password, Snmpv3SecurityParams &params);
};

}

#endif
