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
 * @param length    Data payload length
 * @param password  Password to use for encryption
 * @param params    Security parameters
 * @return The encrypted payload encapsulated in an OCTET STRING
 */
std::shared_ptr<BerOctetString> Snmpv3PrivDES::encrypt(const u8 *data, u32 length, const std::string &password, Snmpv3SecurityParams &params) {
    return nullptr;
}

/**
 * @brief Decrypt a payload using CBC-DES
 * @param data      Data payload to be decrypted
 * @param password  Password to use for decryption
 * @param params    Security parameters
 * @param length    Data payload length
 * @return The decrypted payload
 */
std::unique_ptr<u8> Snmpv3PrivDES::decrypt(std::shared_ptr<BerOctetString> data, const std::string &password, Snmpv3SecurityParams &params, u32 *length) {
    return nullptr;
}

}
