/**
 * @file TlsSocket.cpp
 * @brief TLS Socket wrapper
 */

// https://github.com/ARMmbed/mbedtls/blob/development/programs/ssl/dtls_server.c
// https://tls.mbed.org/discussions/generic/how-to-establish-a-handshake-with-two-way-authentication

// Includes C/C++
#include <stdexcept>

// Own includes
#include "socket/TlsSocket.h"

namespace NetMan {

TlsSocket::TlsSocket() {
    mbedtls_net_init(&fd);
}

TlsSocket::~TlsSocket() {

}

void TlsSocket::bindTo(u16 port) {
    if(mbedtls_net_bind(&fd, NULL, std::to_string(port).c_str(), MBEDTLS_NET_PROTO_TCP) != 0) {
        throw std::runtime_error("Can't bind TLS socket");
    }
}

void TlsSocket::initialize(const std::string &seed) {

    // Initialize SSL context
    mbedtls_ssl_init(&sslContext);
    mbedtls_ssl_config_init(&sslConfig);
    mbedtls_x509_crt_init(&certChain);
    mbedtls_pk_init(&privKey);
    mbedtls_entropy_init(&entropyContext);
    mbedtls_ctr_drbg_init(&drbgContext);

    // Initialize seed
    if(mbedtls_ctr_drbg_seed(&drbgContext, mbedtls_entropy_func, &entropyContext, (u8*) seed.c_str(), seed.length()) != 0) {
        throw std::runtime_error("mbedtls_ctr_drbg_seed");
    }
}

void TlsSocket::addServerCert(const std::string &cert, const std::string &key, const std::string &keypass) {

    // Load certificate
    try {
        TlsSocket::addCert(cert);
    } catch (std::runtime_error &e) {
        throw;
    }

    // Load key
    if(mbedtls_pk_parse_keyfile(&privKey, key.c_str(), keypass.c_str()) != 0) {
        throw std::runtime_error("Error loading private key: " + key);
    }
}

void TlsSocket::addCert(const std::string &cert) {

    // Load certificate
    if(mbedtls_x509_crt_parse_file(&certChain, cert.c_str()) != 0) {
        throw std::runtime_error("Error loading certificate: " + cert);
    }
}

void TlsSocket::finish() {

}

}
