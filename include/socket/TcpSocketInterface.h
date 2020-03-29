/**
 * @file TcpSocketInterface.h
 * @brief Interface for TCP sockets
 */

#ifndef _TCP_SOCKET_INTERFACE_H_
#define _TCP_SOCKET_INTERFACE_H_

// Includes C/C++
#include <memory>

// Includes 3DS
#include <3ds/types.h>

// Defines
#define DEFAULT_TCP_SOCKET_TIMEOUT_SECS		5
#define DEFAULT_TCP_SOCKET_TIMEOUT_USECS	0

namespace NetMan {

/**
 * @class TcpSocketInterface
 */
class TcpSocketInterface {
    public:
        virtual void connectToHost(const struct addrinfo &addr) = 0;
		virtual void sendData(void *data, u32 size) = 0;
		virtual u32 recvData(void *data, u32 size) = 0;
        virtual void bindTo(u16 port) = 0;
		virtual bool dataReceived() = 0;
		virtual void listenState(u32 queueLength) = 0;
		virtual std::shared_ptr<TcpSocketInterface> acceptConnection() = 0;
};

}

#endif
