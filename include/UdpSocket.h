/**
 * @file UdpSocket.h
 * @brief 
 */

#ifndef UDPSOCKET_H_
#define UDPSOCKET_H_

#include <3ds/types.h>

namespace NetMan {

class UdpSocket {
	private:
		int fd;			/**< File descriptor for UDP socket */
	public:
		UdpSocket();
		void sendPacket(void *data, u32 size, const char *ip, u16 port);
		void recvPacket(void *data, u32 size, const char *ip, u16 port);
		virtual ~UdpSocket();
};

}

#endif
