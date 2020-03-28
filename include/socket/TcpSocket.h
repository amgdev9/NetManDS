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

// Includes 3DS
#include <3ds/types.h>

// Defines
#define DEFAULT_TCP_SOCKET_TIMEOUT_SECS		5
#define DEFAULT_TCP_SOCKET_TIMEOUT_USECS	0

namespace NetMan {

/**
 * @class TcpSocket
 */
class TcpSocket {
	protected:
		int fd;					/**< File descriptor for UDP socket */
		struct timeval tv;		/**< Timeout for UDP socket */
	public:
		TcpSocket(int fd, u32 timeoutSecs, u32 timeoutUsecs);
		TcpSocket(u32 timeoutSecs = DEFAULT_TCP_SOCKET_TIMEOUT_SECS, u32 timeoutUsecs = DEFAULT_TCP_SOCKET_TIMEOUT_USECS);
		TcpSocket(const struct addrinfo &addr, u32 timeoutSecs = DEFAULT_TCP_SOCKET_TIMEOUT_SECS, u32 timeoutUsecs = DEFAULT_TCP_SOCKET_TIMEOUT_USECS);
		void connectToHost(const struct addrinfo &addr);
		void sendData(void *data, u32 size);
		u32 recvData(void *data, u32 size);
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
