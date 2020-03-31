/**
 * @file TlsSocket.cpp
 * @brief TLS Socket wrapper
 */

// Own includes
#include "socket/TlsSocket.h"

namespace NetMan {

TlsSocket::TlsSocket(int fd, u32 timeoutSecs, u32 timeoutUsecs) : TcpSocket(fd, timeoutSecs, timeoutUsecs) {

}

TlsSocket::TlsSocket(u32 timeoutSecs, u32 timeoutUsecs) : TcpSocket(timeoutSecs, timeoutUsecs) {

}

TlsSocket::TlsSocket(const struct addrinfo &addr, u32 timeoutSecs, u32 timeoutUsecs) : TcpSocket(addr, timeoutSecs, timeoutUsecs) {

}

void TlsSocket::connectToHost(const struct addrinfo &addr) {

}

void TlsSocket::sendData(void *data, u32 size) {

}

u32 TlsSocket::recvData(void *data, u32 size) {
    return 0;
}

TlsSocket::~TlsSocket() {

}

}
