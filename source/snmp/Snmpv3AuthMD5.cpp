/**
 * @file Snmpv3AuthMD5.cpp
 * @brief SNMPv3 Authentication using MD5
 */

// Includes C/C++
#include <string.h>

// Includes mbedtls
#include <mbedtls/md5.h>

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
 * @param length    Length of the data
 * @param params    Security parameters
 * @param keyptr    Pointer to the user key
 * @note The hash is stored in the security parameters
 */
void Snmpv3AuthMD5::createHash(const u8 *data, u32 length, Snmpv3SecurityParams &params, std::shared_ptr<u8> keyptr) {

    try {

        // Generate the key
        u8 digest[16];      // Final digest
        std::unique_ptr<u8> message(new u8[SNMPV3_AUTH_KEYLENGTH + length]);
        u8 *messagePtr = message.get();
        u8 *key = keyptr.get();

        // Generate K1
        u8 k[SNMPV3_AUTH_KEYLENGTH];
        u8 filler[SNMPV3_AUTH_KEYLENGTH];
        memset(filler, 0x36, SNMPV3_AUTH_KEYLENGTH);
        for(u8 i = 0; i < SNMPV3_AUTH_KEYLENGTH; i++) {
            k[i] = key[i] ^ filler[i];
        }

        // Perform MD5(K1 + data)
        memcpy(messagePtr, k, SNMPV3_AUTH_KEYLENGTH);
        memcpy(messagePtr + SNMPV3_AUTH_KEYLENGTH, data, length);
        mbedtls_md5_context MD;
        mbedtls_md5_init(&MD);
        mbedtls_md5_starts_ret(&MD);
        mbedtls_md5_update_ret(&MD, messagePtr, SNMPV3_AUTH_KEYLENGTH + length);
        mbedtls_md5_finish_ret(&MD, digest);
        mbedtls_md5_free(&MD);

        // Generate K2
        memset(filler, 0x5C, SNMPV3_AUTH_KEYLENGTH);
        for(u8 i = 0; i < SNMPV3_AUTH_KEYLENGTH; i++) {
            k[i] = key[i] ^ filler[i];
        }

        // Perform MD5(K2 + previous MD5)
        memcpy(messagePtr, k, SNMPV3_AUTH_KEYLENGTH);
        memcpy(messagePtr + SNMPV3_AUTH_KEYLENGTH, digest, 16);
        mbedtls_md5_starts_ret(&MD);
        mbedtls_md5_update_ret(&MD, messagePtr, SNMPV3_AUTH_KEYLENGTH + 16);
        mbedtls_md5_finish_ret(&MD, digest);
        mbedtls_md5_free(&MD);

        // Save the final hash (its 12 first octets)
        params.msgAuthenticationParameters = std::string((char*)digest, 12);

    } catch(const std::bad_alloc &e) {
        throw;
    } catch(const std::runtime_error &e) {
        throw;
    }
}

/**
 * @brief Get a key from a password
 * @param password  Password
 * @param params    Security parameters
 * @return The key for that password
 */
std::shared_ptr<u8> Snmpv3AuthMD5::passwordToKey(const std::string &password, Snmpv3SecurityParams &params) {

    // Adapted from https://tools.ietf.org/html/rfc3414#appendix-A.2.1

    // Variables
    mbedtls_md5_context MD;
    u_char *cp, password_buf[64];
    u_long password_index = 0;
    u_long count = 0, i;

    // Create the key buffer
    try {
        std::shared_ptr<u8> keybuffer(new u8[SNMPV3_AUTH_KEYLENGTH]);
        u8 *key = keybuffer.get();
        memset(key, 0, SNMPV3_AUTH_KEYLENGTH);

        // Initialize MD5 context
        mbedtls_md5_init(&MD);
        mbedtls_md5_starts_ret(&MD);

        /**********************************************/
        /* Use while loop until we've done 1 Megabyte */
        /**********************************************/
        while (count < 1048576) {
            cp = password_buf;
            for (i = 0; i < 64; i++) {
                /*************************************************/
                /* Take the next octet of the password, wrapping */
                /* to the beginning of the password as necessary.*/
                /*************************************************/
                *cp++ = password[password_index++ % password.length()];
            }
            mbedtls_md5_update_ret(&MD, password_buf, 64);
            count += 64;
        }
        mbedtls_md5_finish_ret(&MD, key);
        mbedtls_md5_free(&MD);

        /*****************************************************/
        /* Now localize the key with the engineID and pass   */
        /* through MD5 to produce final key                  */
        /* May want to ensure that engineLength <= 32,       */
        /* otherwise need to use a buffer larger than 64     */
        /*****************************************************/
        std::string &engineID = params.msgAuthoritativeEngineID;
        memcpy(password_buf, key, 16);
        memcpy(password_buf + 16, engineID.c_str(), engineID.length());
        memcpy(password_buf + 16 + engineID.length(), key, 16);

        mbedtls_md5_starts_ret(&MD);
        mbedtls_md5_update_ret(&MD, password_buf, 32 + engineID.length());
        mbedtls_md5_finish_ret(&MD, key);
        mbedtls_md5_free(&MD);

        // Return the created key
        return keybuffer;
    } catch (const std::bad_alloc &e) {
        throw;
    } catch (const std::runtime_error &e) {
        throw;
    }
}

}
