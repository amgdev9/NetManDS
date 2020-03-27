/**
 * @file SshHelper.cpp
 * @brief Helper for SSH connections
 */

// Includes C/C++
#include <stdexcept>

// Own includes
#include "ssh/SshHelper.h"

namespace NetMan {

SshHelper::SshHelper() {

    if(libssh2_init(0)) {
        throw std::runtime_error("libssh2_init");
    }

    this->sock = nullptr;
    this->serverData = "";
}

std::shared_ptr<TcpSocket> SshHelper::connect(const std::string &hostname, u16 port, const std::string &username, const std::string &password) {

    struct addrinfo hints;
    struct addrinfo *res = NULL;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_ADDRCONFIG;

    if(getaddrinfo(hostname.c_str(), std::to_string(port).c_str(), &hints, &res) != 0) {
		throw std::runtime_error("Host not found: " + hostname);
	}

    try {
        this->sock = std::make_shared<TcpSocket>(*res, 5);
        this->sock->connectToHost(*res);

        session = libssh2_session_init();
	    if(!session) {
            throw std::runtime_error("libssh2_session_init");
        }

	    libssh2_session_set_timeout(session, 10000);

	    if(libssh2_session_handshake(session, sock->getDescriptor())) {
            throw std::runtime_error("libssh2_session_handshake: " + std::to_string(libssh2_session_last_errno(session)));
        }

        std::string userauthlist = "";
        char *userauth = libssh2_userauth_list(session, username.c_str(), username.length());
        if(userauth) {
            userauthlist = std::string(userauth);
        }

	    if(userauthlist.find("password") == std::string::npos) {
            throw std::runtime_error("Password authentication not supported by server");
        }

		if(libssh2_userauth_password(session, username.c_str(), password.c_str())) {
            throw std::runtime_error("Incorrect password");
        }

        channel = libssh2_channel_open_session(session);
        if(!channel) {
            throw std::runtime_error("libssh2_channel_open_session");
        }

        if(libssh2_channel_request_pty(channel, "xterm-256color")) {
            throw std::runtime_error("libssh2_channel_request_pty");
        }

	    if(libssh2_channel_shell(channel)) {
            throw std::runtime_error("libssh2_channel_shell");
        }
 
	    libssh2_channel_set_blocking(channel, 0);
        libssh2_channel_flush(channel);

        return this->sock;
	 
    } catch (const std::bad_alloc &e) {
        throw;
    } catch (const std::runtime_error &e) {
        throw;
    }
}

bool SshHelper::sendData(const std::string &data) {
			
	if (data.compare(SSH_EXIT_COMMAND) == 0) {
        return true;
    }
			
	u32 written = 0;
    int rc = 0;
	do {
	    rc = libssh2_channel_write(channel, data.c_str(), data.length());
		written += rc;
	} while (LIBSSH2_ERROR_EAGAIN != rc && rc > 0 && written != data.length());

	if (rc < 0 && LIBSSH2_ERROR_EAGAIN != rc)  {
        throw std::runtime_error("libssh2_channel_write");
	}

    return false;
}

bool SshHelper::recvData() {

    if(!this->sock->dataReceived()) {
        return false;
    }

    int rc = 0;
    std::unique_ptr<char> inputbuf = nullptr;

    try {
        inputbuf = std::unique_ptr<char>(new char[SSH_INPUT_LENGTH]);
    } catch(const std::bad_alloc &e) {
        throw;
    }

    this->serverData.clear();
    do {
        memset(inputbuf.get(), 0, SSH_INPUT_LENGTH);
		rc = libssh2_channel_read(channel, inputbuf.get(), SSH_INPUT_LENGTH);
        this->serverData += inputbuf.get();
	} while (LIBSSH2_ERROR_EAGAIN != rc && rc > 0);
		
	if(rc < 0 && LIBSSH2_ERROR_EAGAIN != rc) {
		throw std::runtime_error("libssh2_channel_read");
	}

    return true;   
}

void SshHelper::disconnect() {

    if(channel) {
        libssh2_channel_free(channel);
    }

    libssh2_session_disconnect(session, "Normal shutdown");
    libssh2_session_free(session);
}

SshHelper::~SshHelper() {
    libssh2_exit();
}

}
