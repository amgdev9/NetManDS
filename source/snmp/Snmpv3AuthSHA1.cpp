/**
 * @file Snmpv3AuthSHA1.cpp
 * @brief SNMPv3 Authentication using SHA1
 */

// Own includes
#include "snmp/Snmpv3AuthSHA1.h"

namespace NetMan {

/**
 * @brief Constructor for a Snmpv3AuthSHA1
 */
Snmpv3AuthSHA1::Snmpv3AuthSHA1() { }

/**
 * @brief Destructor for a Snmpv3Auth
*/
Snmpv3AuthSHA1::~Snmpv3AuthSHA1() { }

/**
 * @brief Create a HMAC-96-SHA1 hash
 * @param data      Data to be hashed
 * @param password  Password for the HMAC
 * @param params    Security parameters
 * @note The hash is stored in the security parameters
 */
void Snmpv3AuthSHA1::createHash(std::unique_ptr<u8> data, const std::string &password, Snmpv3SecurityParams &params) {

}

/**
 * @brief Authenticate a SNMPv3 PDU
 * @param data      Data to be authenticated
 * @param password  Password for the HMAC
 * @param params    Security parameters
 * @return Whether the authentication was successful or not
 */
bool Snmpv3AuthSHA1::authenticate(std::unique_ptr<u8> data, const std::string &password, Snmpv3SecurityParams &params) {
    return false;
}

}
