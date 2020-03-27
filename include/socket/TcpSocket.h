/**
 * @file TcpSocket.h
 * @brief TCP Socket wrapper
 */

#ifndef TCPSOCKET_H_
#define TCPSOCKET_H_

// Includes C/C++
#include <sys/socket.h>
#include <netdb.h>
#include <memory>

// Own includes
#include "socket/Socket.h"

namespace NetMan {

/**
 * @class TcpSocket
 */
class TcpSocket : public Socket {
	private:
		int fd;					/**< File descriptor for UDP socket */
		struct timeval tv;		/**< Timeout for UDP socket */
	public:
		TcpSocket(int fd, u32 timeoutSecs, u32 timeoutUsecs);
		TcpSocket(u32 timeoutSecs = DEFAULT_SOCKET_TIMEOUT_SECS, u32 timeoutUsecs = DEFAULT_SOCKET_TIMEOUT_USECS);
		TcpSocket(const struct addrinfo &addr, u32 timeoutSecs = DEFAULT_SOCKET_TIMEOUT_SECS, u32 timeoutUsecs = DEFAULT_SOCKET_TIMEOUT_USECS);
		void connectToHost(const struct addrinfo &addr);
		void sendPacket(void *data, u32 size, const std::string &ip = "", u16 port = 0);
		u32 recvPacket(void *data, u32 size, const std::string &ip = "", u16 port = 0);
		void listenState(u32 queueLength);
		std::shared_ptr<TcpSocket> acceptConnection();
		void bindTo(u16 port);
		bool dataReceived();
		inline int getDescriptor() { return this->fd; }
		inline void setTimeout(u32 secs, u32 usecs) { tv.tv_sec = secs; tv.tv_usec = usecs; }
		virtual ~TcpSocket();
};

}

#endif
