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

namespace NetMan {

/**
 * @class TcpSocket
 */
class TcpSocket {
	protected:
		int fd;					/**< File descriptor for TCP socket */
		struct timeval tv;		/**< Timeout for TCP socket */
	public:
		TcpSocket(int fd, u32 timeoutSecs);
		TcpSocket(u32 timeoutSecs);
		TcpSocket(const struct addrinfo &addr, u32 timeoutSecs);
		virtual void connectToHost(const struct addrinfo &addr);
		virtual void sendData(void *data, u32 size);
		virtual u32 recvData(void *data, u32 size);
        void bindTo(u16 port);
		bool dataReceived();
		inline int getDescriptor() { return this->fd; }
		inline void setTimeout(u32 secs, u32 usecs) { tv.tv_sec = secs; tv.tv_usec = usecs; }
		void listenState(u32 queueLength);
		std::shared_ptr<TcpSocket> acceptConnection(u32 timeout);
		virtual ~TcpSocket();
};

}

#endif
