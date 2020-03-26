/**
 * @file Socket.h
 * @brief Socket interface
 */

#ifndef SOCKET_H_
#define SOCKET_H_

// Includes C/C++
#include <string>

// Includes 3DS
#include <3ds/types.h>

// Defines
#define DEFAULT_SOCKET_TIMEOUT_SECS		5
#define DEFAULT_SOCKET_TIMEOUT_USECS	0

namespace NetMan {

/**
 * @class Socket
 */
class Socket {
	public:
		virtual void sendPacket(void *data, u32 size, const std::string &ip, u16 port) = 0;
		virtual u32 recvPacket(void *data, u32 size, const std::string &ip = "", u16 port = 0) = 0;
		virtual void bindTo(u16 port) = 0;
};

}

#endif
