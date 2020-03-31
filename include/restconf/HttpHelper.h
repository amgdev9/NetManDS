/**
 * @file HttpHelper.h
 * @brief Helper for HTTP requests
 */
#ifndef _HTTP_HELPER_H_
#define _HTTP_HELPER_H_

// Own includes
#include "socket/TcpSocket.h"

namespace NetMan {

/**
 * @class HttpHelper
 */
class HttpHelper {
    private:
        std::string headers;
    public:
        HttpHelper(std::shared_ptr<TcpSocket> sock);
        void addHeader(const std::string &name, const std::string &value);
        void sendRequest(const std::string &host, u16 port);
        void recvResponse();
        virtual ~HttpHelper();
};

}

#endif
