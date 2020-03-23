/**
 * @file Snmpv3AuthMD5.cpp
 * @brief SNMPv3 Authentication using MD5
 */

// Own includes
#include "snmp/Snmpv3AuthMD5.h"

namespace NetMan {

/**
 * @brief Constructor for a Snmpv3AuthMD5
 */
Snmpv3AuthMD5::Snmpv3AuthMD5() { }

/**
 * @brief Destructor for a Snmpv3AuthMD5
 */
Snmpv3AuthMD5::~Snmpv3AuthMD5() { }

/**
 * @brief Create a HMAC-96-MD5 hash
 * @param data      Data to be hashed
 * @param password  Password for the HMAC
 * @param params    Security parameters
 * @note The hash is stored in the security parameters
 */
void Snmpv3AuthMD5::createHash(std::unique_ptr<u8> data, const std::string &password, Snmpv3SecurityParams &params) {

}

/**
 * @brief Authenticate a SNMPv3 PDU
 * @param data      Data to be authenticated
 * @param password  Password for the HMAC
 * @param params    Security parameters
 * @return Whether the authentication was successful or not
 */
bool Snmpv3AuthMD5::authenticate(std::unique_ptr<u8> data, const std::string &password, Snmpv3SecurityParams &params) {
    return false;
}

}
