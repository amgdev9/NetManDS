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
#include "UdpSocket.h"

namespace NetMan {

/**
 * @brief Constructor for a UDP socket
 * @param timeoutSecs Timeout, in seconds
 * @param timeoutUsecs Remaining microseconds of the timeout
 */
UdpSocket::UdpSocket(u32 timeoutSecs, u32 timeoutUsecs) {

	this->fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);

	if(this->fd < 0) {
		throw std::runtime_error("socket() failed");
	}

	this->tv.tv_sec = timeoutSecs;
	this->tv.tv_usec = timeoutUsecs;
}

/**
 * @brief Send a UDP datagram
 * @param data Data to be sent
 * @param size Size of the outcoming data
 * @param ip IPv4 address of the destination
 * @param port Destination port
 */
void UdpSocket::sendPacket(void *data, u32 size, const std::string &ip, u16 port) {

	struct sockaddr_in dest;
	memset(&dest, 0, sizeof(sockaddr_in));
	dest.sin_family = AF_INET;
	dest.sin_port = htons(port);
	dest.sin_addr.s_addr = inet_addr(&ip[0]);

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
 */
void UdpSocket::recvPacket(void *data, u32 size, const std::string &ip, u16 port) {

	struct sockaddr_in src;
	socklen_t src_len = sizeof(src);

	fd_set set;
	FD_ZERO(&set);
	FD_SET(this->fd, &set);

	if(select(this->fd + 1, &set, NULL, NULL, &this->tv) <= 0 || !FD_ISSET(this->fd, &set)) {
		throw std::runtime_error("Socket timeout");
	}

	if(recvfrom(this->fd, data, size, 0, (struct sockaddr*)&src, &src_len) <= 0) {
		throw std::runtime_error("recvfrom() failed");
	}

	if(ip.compare("") != 0 && src.sin_addr.s_addr != inet_addr(&ip[0])) {
		throw std::runtime_error("Source IP does not match");
	}

	if(port && src.sin_port != htons(port)) {
		throw std::runtime_error("Source port does not match");
	}
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
