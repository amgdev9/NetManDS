/**
 * @file Snmpv3AuthProto.cpp
 * @brief SNMPv3 Authentication protocol interface
 */

// Own includes
#include "snmp/Snmpv3AuthProto.h"

namespace NetMan {

/**
 * @brief Authenticate a SNMPv3 PDU
 * @param data      Data to be authenticated
 * @param length    Data length
 * @param password  Password for the HMAC
 * @param params    Security parameters
 * @return Whether the authentication was successful or not
 */
bool Snmpv3AuthProto::authenticate(const u8 *data, u32 length, const std::string &password, Snmpv3SecurityParams &params) {

    std::string digest = params.msgAuthenticationParameters;

    try {

        // Generate the SHA1 hash
        this->createHash(data, length, password, params);

        // Compare it with the previous one
        return (digest.compare(params.msgAuthenticationParameters) == 0);
    } catch(const std::bad_alloc &e) {
        throw;
    } catch(const std::runtime_error &e) {
        throw;
    }
}

}
