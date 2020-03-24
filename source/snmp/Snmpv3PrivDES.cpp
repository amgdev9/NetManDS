/**
 * @file Snmpv3PrivProto.cpp
 * @brief SNMPv3 Privacy protocol using DES
 */

// Includes C/C++
#include <time.h>

// Includes mbedtls
#include <mbedtls/des.h>

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
 * @param params    Security parameters
 * @param userKey   User key
 * @return The encrypted payload encapsulated in an OCTET STRING
 */
std::shared_ptr<BerOctetString> Snmpv3PrivDES::encrypt(const u8 *data, u32 length, Snmpv3SecurityParams &params, std::shared_ptr<u8> userKey) {
    
    // Get DES key
    u8 *userKeyPtr = userKey.get();
    u8 *encKey = userKeyPtr;
    u8 *preIV = &userKeyPtr[8];

    // Get salt
    u8 salt[8];
    u8 *engineBoots = (u8*)&params.msgAuthoritativeEngineBoots;
    for(u8 i = 0; i < 4; i++){
        salt[i] = engineBoots[3 - i];   // In Big Endian
    }
    for(u8 i = 4; i < 8; i++) {
        salt[i] = rand() %0xFF;
    }

    // Save the salt
    params.msgPrivacyParameters = std::string((char*)salt, 8);

    // Calculate IV using preIV and salt
    u8 iv[8];
    for(u8 i = 0; i < 8; i++) {
        iv[i] = preIV[i] ^ salt[i];
    }

    try {
        // Create a buffer for the encrypted data
        std::unique_ptr<u8> encryptedData(new u8[length]);

        // Encrypt data
        mbedtls_des_context ctx;
        mbedtls_des_init(&ctx);
        mbedtls_des_setkey_enc(&ctx, encKey);
        int res = mbedtls_des_crypt_cbc(&ctx, MBEDTLS_DES_ENCRYPT, length, iv, data, encryptedData.get());
        mbedtls_des_free(&ctx);

        // Check decrypt status
        if(res != 0) {
            throw std::runtime_error("Error encrypting DES data. Error code " + std::to_string(res));
        }

        // Return encrypted data
        return std::make_shared<BerOctetString>(std::string((char*)encryptedData.get(), length));
    } catch (const std::bad_alloc &e) {
        throw;
    } catch (const std::runtime_error &e) {
        throw;
    }
}

/**
 * @brief Decrypt a payload using CBC-DES
 * @param data      Data payload to be decrypted
 * @param params    Security parameters
 * @param userKey   User key
 * @return The decrypted payload
 */
std::unique_ptr<u8> Snmpv3PrivDES::decrypt(std::shared_ptr<BerOctetString> data, Snmpv3SecurityParams &params, std::shared_ptr<u8> userKey) {
    
    // Get DES key
    u8 *userKeyPtr = userKey.get();
    u8 *preIV = &userKeyPtr[8];
    u8 *decKey = userKeyPtr;

    try {
        // Get salt
        if(params.msgPrivacyParameters.length() != 8) {
            throw std::runtime_error("privParam length is not 8");
        }
        u8 *salt = (u8*)params.msgPrivacyParameters.c_str();

        // Calculate IV using preIV and salt
        u8 iv[8];
        for(u8 i = 0; i < 8; i++) {
            iv[i] = preIV[i] ^ salt[i];
        }

        // Create a buffer for the decrypted data
        u32 length = data->getValue().length();
        std::unique_ptr<u8> decryptedData(new u8[length]);

        // Decrypt data
        mbedtls_des_context ctx;
        mbedtls_des_init(&ctx);
        mbedtls_des_setkey_dec(&ctx, decKey);
        int res = mbedtls_des_crypt_cbc(&ctx, MBEDTLS_DES_DECRYPT, length, iv, (u8*)data->getValue().c_str(), decryptedData.get());
        mbedtls_des_free(&ctx);

        // Check decrypt status
        if(res != 0) {
            throw std::runtime_error("Error decrypting DES data. Error code " + std::to_string(res));
        }

        // Return decrypted data
        return decryptedData;
    } catch (const std::bad_alloc &e) {
        throw;
    } catch (const std::runtime_error &e) {
        throw;
    }
}

}
