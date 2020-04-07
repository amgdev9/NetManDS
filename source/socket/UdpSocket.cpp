/**
 * @file UdpSocket.cpp
 * @brief UDP socket wrapper
 */

// Includes C/C++
#include <unistd.h>
#include <string.h>
#include <stdexcept>
#include <arpa/inet.h>

// Own includes
#include "socket/UdpSocket.h"

namespace NetMan {

/**
 * @brief Constructor for a UDP socket
 * @param timeoutSecs Timeout, in seconds
 */
UdpSocket::UdpSocket(u32 timeoutSecs) {

	this->fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);

	if(this->fd < 0) {
		throw std::runtime_error("socket() failed");
	}

	this->tv.tv_sec = timeoutSecs;
	this->tv.tv_usec = 0;
	this->lastOrigin = 0;
	this->lastPort = 0;
}

/**
 * @brief Send a UDP datagram
 * @param data Data to be sent
 * @param size Size of the outcoming data
 * @param ip IPv4 address of the destination. If zero, last received packet's origin IP-port will be used
 * @param port Destination port
 */
void UdpSocket::sendPacket(void *data, u32 size, in_addr_t ip, u16 port) {

	struct sockaddr_in dest;
	memset(&dest, 0, sizeof(sockaddr_in));
	dest.sin_family = AF_INET;
	if(ip == 0) {
		dest.sin_addr.s_addr = this->lastOrigin;
		dest.sin_port = this->lastPort;
	} else {
		dest.sin_addr.s_addr = ip;
		dest.sin_port = htons(port);
	}

	if(sendto(this->fd, data, size, 0, (const struct sockaddr*)&dest, sizeof(dest)) < 0) {
		throw std::runtime_error("sendto() failed");
	}
}

/**
 * @brief Receive a UDP datagram
 * @param data Data to be received
 * @param size Size of the incoming data
 * @param ip IPv4 address of the source
 * @param port Expected source port
 * @return The number of bytes received
 */
u32 UdpSocket::recvPacket(void *data, u32 size, in_addr_t ip, u16 port) {

	struct sockaddr_in src;
	socklen_t src_len = sizeof(src);

	fd_set set;
	FD_ZERO(&set);
	FD_SET(this->fd, &set);

	if(select(this->fd + 1, &set, NULL, NULL, &this->tv) <= 0 || !FD_ISSET(this->fd, &set)) {
		throw std::runtime_error("Socket timeout");
	}

	u32 recvSize;
	if((recvSize = recvfrom(this->fd, data, size, 0, (struct sockaddr*)&src, &src_len)) <= 0) {
		throw std::runtime_error("recvfrom() failed");
	}

	if(ip != 0 && src.sin_addr.s_addr != ip) {
		throw std::runtime_error("Source IP does not match");
	}

	if(port && src.sin_port != htons(port)) {
		throw std::runtime_error("Source port does not match");
	}

	this->lastOrigin = src.sin_addr.s_addr;
	this->lastPort = src.sin_port;

	return recvSize;
}

/**
 * @brief Bind a socket to a port
 * @param port Port to bind
 */
void UdpSocket::bindTo(u16 port) {

	struct sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	server.sin_addr.s_addr = INADDR_ANY;

	if(bind(this->fd, (struct sockaddr*)&server, sizeof(server)) < 0) {
		throw std::runtime_error("Could not bind to port");
	}
}

/**
 * @brief Destructor for a socket
 */
UdpSocket::~UdpSocket() {
	close(this->fd);
}

}
