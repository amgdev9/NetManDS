/**
 * @file TlsSocket.h
 * @brief TLS Socket wrapper
 */

#ifndef TLSSOCKET_H_
#define TLSSOCKET_H_

// Own includes
#include "socket/TcpSocket.h"

namespace NetMan {

/**
 * @class TlsSocket
 */
class TlsSocket : public TcpSocket {
	public:
		TlsSocket(int fd, u32 timeoutSecs, u32 timeoutUsecs);
		TlsSocket(u32 timeoutSecs = DEFAULT_TCP_SOCKET_TIMEOUT_SECS, u32 timeoutUsecs = DEFAULT_TCP_SOCKET_TIMEOUT_USECS);
		TlsSocket(const struct addrinfo &addr, u32 timeoutSecs = DEFAULT_TCP_SOCKET_TIMEOUT_SECS, u32 timeoutUsecs = DEFAULT_TCP_SOCKET_TIMEOUT_USECS);
		void connectToHost(const struct addrinfo &addr) override;
		void sendData(void *data, u32 size) override;
		u32 recvData(void *data, u32 size) override;
		virtual ~TlsSocket();
};

}

#endif
