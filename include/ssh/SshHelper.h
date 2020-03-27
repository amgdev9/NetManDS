/**
 * @file SshHelper.h
 * @brief Helper for SSH connections
 */

// Includes C/C++
#include <string>
#include <memory>

// Includes SSH
#include <libssh2.h>

// Includes 3DS
#include <3ds/types.h>

// Own includes
#include "socket/TcpSocket.h"

// Defines
#define SSH_EXIT_COMMAND    "exit"
#define SSH_INPUT_LENGTH    0x1000

namespace NetMan {

/**
 * @class SshHelper
 */
class SshHelper {
    private:
        LIBSSH2_SESSION *session;
	    LIBSSH2_CHANNEL *channel;
        std::shared_ptr<TcpSocket> sock;
        std::string serverData;
    public:
        SshHelper();
        std::shared_ptr<TcpSocket> connect(const std::string &hostname, u16 port, const std::string &username, const std::string &password);
        void disconnect();
        bool sendData(const std::string &data);
        bool recvData();
        inline std::string &getRecvData() { return this->serverData; }
        ~SshHelper();
};

}
