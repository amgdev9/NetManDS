/**
 * @file Snmpv3AuthSHA1.cpp
 * @brief SNMPv3 Authentication using SHA1
 */

// Includes C/C++
#include <string.h>

// Includes mbedtls
#include <mbedtls/sha1.h>

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
 * @param length    Length of the data
 * @param password  Password for the HMAC
 * @param params    Security parameters
 * @note The hash is stored in the security parameters
 */
void Snmpv3AuthSHA1::createHash(const u8 *data, u32 length, Snmpv3SecurityParams &params, std::shared_ptr<u8> keyptr) {

    try {

        // Generate the key
        u8 digest[20];      // Final digest
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

        // Perform SHA1(K1 + data)
        memcpy(messagePtr, k, SNMPV3_AUTH_KEYLENGTH);
        memcpy(messagePtr + SNMPV3_AUTH_KEYLENGTH, data, length);
        mbedtls_sha1_context SH;
        mbedtls_sha1_init(&SH);
        mbedtls_sha1_starts_ret(&SH);
        mbedtls_sha1_update_ret(&SH, messagePtr, SNMPV3_AUTH_KEYLENGTH + length);
        mbedtls_sha1_finish_ret(&SH, digest);
        mbedtls_sha1_free(&SH);

        // Generate K2
        memset(filler, 0x5C, SNMPV3_AUTH_KEYLENGTH);
        for(u8 i = 0; i < SNMPV3_AUTH_KEYLENGTH; i++) {
            k[i] = key[i] ^ filler[i];
        }

        // Perform SHA1(K2 + previous SHA1)
        memcpy(messagePtr, k, SNMPV3_AUTH_KEYLENGTH);
        memcpy(messagePtr + SNMPV3_AUTH_KEYLENGTH, digest, 20);
        mbedtls_sha1_starts_ret(&SH);
        mbedtls_sha1_update_ret(&SH, messagePtr, SNMPV3_AUTH_KEYLENGTH + 20);
        mbedtls_sha1_finish_ret(&SH, digest);
        mbedtls_sha1_free(&SH);

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
std::shared_ptr<u8> Snmpv3AuthSHA1::passwordToKey(const std::string &password, Snmpv3SecurityParams &params) {
    
    // Adapted from https://tools.ietf.org/html/rfc3414#appendix-A.2.2

    // Variables
    mbedtls_sha1_context SH;
    u_char *cp, password_buf[72];
    u_long password_index = 0;
    u_long count = 0, i;

    // Create the key buffer
    try {
        std::shared_ptr<u8> keybuffer(new u8[SNMPV3_AUTH_KEYLENGTH]);
        u8 *key = keybuffer.get();
        memset(key, 0, SNMPV3_AUTH_KEYLENGTH);

        // Initialize SHA1 context
        mbedtls_sha1_init(&SH);
        mbedtls_sha1_starts_ret(&SH);

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
            mbedtls_sha1_update_ret(&SH, password_buf, 64);
            count += 64;
        }
        mbedtls_sha1_finish_ret(&SH, key);
        mbedtls_sha1_free(&SH);

        /*****************************************************/
        /* Now localize the key with the engineID and pass   */
        /* through SHA1 to produce final key                 */
        /* May want to ensure that engineLength <= 32,       */
        /* otherwise need to use a buffer larger than 64     */
        /*****************************************************/
        std::string &engineID = params.msgAuthoritativeEngineID;
        memcpy(password_buf, key, 20);
        memcpy(password_buf + 20, engineID.c_str(), engineID.length());
        memcpy(password_buf + 20 + engineID.length(), key, 20);

        mbedtls_sha1_starts_ret(&SH);
        mbedtls_sha1_update_ret(&SH, password_buf, 40 + engineID.length());
        mbedtls_sha1_finish_ret(&SH, key);
        mbedtls_sha1_free(&SH);

        // Return the created key
        return keybuffer;
    } catch (const std::bad_alloc &e) {
        throw;
    } catch (const std::runtime_error &e) {
        throw;
    }
}

}
