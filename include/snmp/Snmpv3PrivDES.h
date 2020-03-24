/**
 * @file Snmpv3PrivProto.h
 * @brief SNMPv3 Privacy protocol using DES
 */

#ifndef _SNMPV3_PRIV_DES_H_
#define _SNMPV3_PRIV_DES_H_

// Includes C/C++
#include <string>
#include <memory>

// Includes 3DS
#include <3ds.h>

// Own includes
#include "Snmpv3Pdu.h"
#include "Snmpv3PrivProto.h"

namespace NetMan {

/**
 * @class Snmpv3PrivDES
 */
class Snmpv3PrivDES : public Snmpv3PrivProto {
    public:
        Snmpv3PrivDES();
        virtual ~Snmpv3PrivDES();
        std::shared_ptr<BerOctetString> encrypt(const u8 *data, u32 length, const std::string &password, Snmpv3SecurityParams &params, std::shared_ptr<Snmpv3AuthProto> authProto) override;
        std::unique_ptr<u8> decrypt(std::shared_ptr<BerOctetString> data, u32 *length, const std::string &password, Snmpv3SecurityParams &params, std::shared_ptr<Snmpv3AuthProto> authProto) override;
};

}

#endif
