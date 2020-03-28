/**
 * @file TlsSocket.h
 * @brief TLS Socket wrapper
 */

#ifndef TLSSOCKET_H_
#define TLSSOCKET_H_

// Includes C/C++
#include <string>

// Includes 3DS
#include <3ds/types.h>

// Includes mbedtls
#include <mbedtls/net.h>
#include <mbedtls/ssl.h>
#include <mbedtls/entropy.h>
#include <mbedtls/ctr_drbg.h>
#include <mbedtls/debug.h>

namespace NetMan {

/**
 * @class TlsSocket
 */
class TlsSocket {
	private:
        mbedtls_net_context fd;
        static mbedtls_ssl_context sslContext;
        static mbedtls_ssl_config sslConfig;
        static mbedtls_entropy_context entropyContext;
        static mbedtls_ctr_drbg_context drbgContext;
        static mbedtls_x509_crt certChain;
        static mbedtls_pk_context privKey;
	public:
        static void initialize(const std::string &seed);
        static void addServerCert(const std::string &cert, const std::string &key, const std::string &keypass);
        static void addCert(const std::string &cert);
        static void finish();
		TlsSocket();
        void bindTo(u16 port);
		virtual ~TlsSocket();
};

}

#endif
