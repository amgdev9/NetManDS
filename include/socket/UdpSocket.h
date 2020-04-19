/**
 * @file UdpSocket.h
 * @brief UDP Socket wrapper
 */

#ifndef UDPSOCKET_H_
#define UDPSOCKET_H_

// Includes C/C++
#include <sys/socket.h>
#include <arpa/inet.h>

// Includes 3DS
#include <3ds/types.h>

namespace NetMan {

/**
 * @class UdpSocket
 */
class UdpSocket {
    private:
        int fd;					/**< File descriptor for UDP socket */
        struct timeval tv;		/**< Timeout for UDP socket */
        in_addr_t lastOrigin;	/**< Last received packet's origin IP */
        in_port_t lastPort;		/**< Last received packet's origin port */
    public:
        UdpSocket(u32 timeoutSecs);
        void sendPacket(void *data, u32 size, in_addr_t ip, u16 port);
        u32 recvPacket(void *data, u32 size, in_addr_t ip = 0, u16 port = 0);
        void bindTo(u16 port);
        inline in_addr_t getLastOrigin() { return this->lastOrigin; }
        inline in_port_t getLastPort() { return this->lastPort; }
        inline int getDescriptor() { return fd; }
        virtual ~UdpSocket();
};

}

#endif
