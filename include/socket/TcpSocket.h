/**
 * @file TcpSocket.h
 * @brief TCP Socket wrapper
 */

#ifndef TCPSOCKET_H_
#define TCPSOCKET_H_

// Includes C/C++
#include <sys/socket.h>
#include <netdb.h>

// Own includes
#include "socket/TcpSocketInterface.h"

namespace NetMan {

/**
 * @class TcpSocket
 */
class TcpSocket : public TcpSocketInterface {
	protected:
		int fd;					/**< File descriptor for TCP socket */
		struct timeval tv;		/**< Timeout for TCP socket */
	public:
		TcpSocket(int fd, u32 timeoutSecs, u32 timeoutUsecs);
		TcpSocket(u32 timeoutSecs = DEFAULT_TCP_SOCKET_TIMEOUT_SECS, u32 timeoutUsecs = DEFAULT_TCP_SOCKET_TIMEOUT_USECS);
		TcpSocket(const struct addrinfo &addr, u32 timeoutSecs = DEFAULT_TCP_SOCKET_TIMEOUT_SECS, u32 timeoutUsecs = DEFAULT_TCP_SOCKET_TIMEOUT_USECS);
		void connectToHost(const struct addrinfo &addr) override;
		void sendData(void *data, u32 size) override;
		u32 recvData(void *data, u32 size) override;
        void bindTo(u16 port) override;
		bool dataReceived() override;
		inline int getDescriptor() { return this->fd; }
		inline void setTimeout(u32 secs, u32 usecs) { tv.tv_sec = secs; tv.tv_usec = usecs; }
		void listenState(u32 queueLength) override;
		std::shared_ptr<TcpSocketInterface> acceptConnection() override;
		virtual ~TcpSocket();
};

}

#endif
