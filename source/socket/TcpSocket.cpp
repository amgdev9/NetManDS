/**
 * @file TcpSocket.cpp
 * @brief TCP socket wrapper
 */

// Includes C/C++
#include <unistd.h>
#include <string.h>
#include <stdexcept>
#include <arpa/inet.h>

// Own includes
#include "socket/TcpSocket.h"

namespace NetMan {

/**
 * @brief Constructor for a TCP socket
 * @param addr          Address information
 * @param timeoutSecs   Timeout, in seconds
 * @param timeoutUsecs  Remaining microseconds of the timeout
 */
TcpSocket::TcpSocket(const struct addrinfo &addr, u32 timeoutSecs, u32 timeoutUsecs) {

	this->fd = socket(addr.ai_family, addr.ai_socktype, addr.ai_protocol);

	if(this->fd < 0) {
		throw std::runtime_error("socket() failed");
	}

	this->tv.tv_sec = timeoutSecs;
	this->tv.tv_usec = timeoutUsecs;
}

/**
 * @brief Connect to a host
 * @param addr  Host information
 */
void TcpSocket::connectToHost(const struct addrinfo &addr) {

    if (connect(this->fd, addr.ai_addr, addr.ai_addrlen) != 0) {
		throw std::runtime_error("Error connecting to host");
	}
}

/**
 * @brief Send a TCP packet
 * @param data Data to be sent
 * @param size Size of the outcoming data
 */
void TcpSocket::sendPacket(void *data, u32 size) {

	if(send(this->fd, data, size, 0) < 0) {
		throw std::runtime_error("send() failed");
	}
}

/**
 * @brief Check if any data was received
 * @return Have we received data?
 * @note This method is used by recvPacket() internally
 */
bool TcpSocket::dataReceived() {

    fd_set set;
	FD_ZERO(&set);
	FD_SET(this->fd, &set);

	return !(select(this->fd + 1, &set, NULL, NULL, &this->tv) <= 0 || !FD_ISSET(this->fd, &set));
}

/**
 * @brief Receive a TCP packet
 * @param data Data to be received
 * @param size Size of the incoming data
 * @return The number of bytes received
 */
u32 TcpSocket::recvPacket(void *data, u32 size) {

	if(!this->dataReceived()) {
		throw std::runtime_error("Socket timeout");
	}

	u32 recvSize;
	if((recvSize = recv(this->fd, data, size, 0)) <= 0) {
		throw std::runtime_error("recv() failed");
	}

	return recvSize;
}

/**
 * @brief Bind a socket to a port
 * @param port Port to bind
 */
void TcpSocket::bindTo(u16 port) {

	struct sockaddr_in server;
	server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(port);

	if(bind(this->fd, (struct sockaddr*)&server, sizeof(server)) < 0) {
		throw std::runtime_error("Could not bind to port");
	}
}

/**
 * @brief Destructor for a socket
 */
TcpSocket::~TcpSocket() {
	close(this->fd);
}

}
