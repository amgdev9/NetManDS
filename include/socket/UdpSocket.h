/**
 * @file UdpSocket.h
 * @brief UDP Socket wrapper
 */

#ifndef UDPSOCKET_H_
#define UDPSOCKET_H_

// Includes C/C++
#include <sys/socket.h>

// Own includes
#include "Socket.h"

namespace NetMan {

/**
 * @class UdpSocket
 */
class UdpSocket : public Socket {
	private:
		int fd;					/**< File descriptor for UDP socket */
		struct timeval tv;		/**< Timeout for UDP socket */
		in_addr_t lastOrigin;	/**< Last received packet's origin IP */
		in_port_t lastPort;		/**< Last received packet's origin port */
	public:
		UdpSocket(u32 timeoutSecs = DEFAULT_SOCKET_TIMEOUT_SECS, u32 timeoutUsecs = DEFAULT_SOCKET_TIMEOUT_USECS);
		void sendPacket(void *data, u32 size, const std::string &ip, u16 port) override;
		u32 recvPacket(void *data, u32 size, const std::string &ip = "", u16 port = 0) override;
		void bindTo(u16 port) override;
		virtual ~UdpSocket();
};

}

#endif
