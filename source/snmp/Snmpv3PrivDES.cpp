/**
 * @file Snmpv3PrivProto.cpp
 * @brief SNMPv3 Privacy protocol using DES
 */

// Own includes
#include "snmp/Snmpv3PrivDES.h"

namespace NetMan {

/**
 * @brief Constructor for a Snmpv3PrivDES
 */
Snmpv3PrivDES::Snmpv3PrivDES() { }

/**
 * @brief Destructor for a Snmpv3PrivDES
 */
Snmpv3PrivDES::~Snmpv3PrivDES() { }

/**
 * @brief Encrypt a payload using CBC-DES
 * @param data      Data payload to be encrypted
 * @param password  Password to use for encryption
 * @param params    Security parameters
 * @return The encrypted payload encapsulated in an OCTET STRING
 */
std::shared_ptr<BerOctetString> Snmpv3PrivDES::encrypt(std::unique_ptr<u8> data, const std::string &password, Snmpv3SecurityParams &params) {
    return nullptr;
}

/**
 * @brief Decrypt a payload using CBC-DES
 * @param data      Data pauload to be decrypted
 * @param password  Password to use for decryption
 * @param params    Security parameters
 * @return The decrypted payload
 */
std::unique_ptr<u8> Snmpv3PrivDES::decrypt(std::shared_ptr<BerOctetString> data, const std::string &password, Snmpv3SecurityParams &params) {
    return nullptr;
}

}
