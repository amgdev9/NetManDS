/**
 * @file HttpHelper.cpp
 * @brief Helper for HTTP requests
 */

// Own includes
#include "restconf/HttpHelper.h"

namespace NetMan {

HttpHelper::HttpHelper(std::shared_ptr<TcpSocket> sock) {
    headers = std::string();
}

void HttpHelper::addHeader(const std::string &name, const std::string &value) {
    headers.append(name + ": " + value + "\n");
}

void HttpHelper::sendRequest(const std::string &host, u16 port) {

}

void HttpHelper::recvResponse() {
    
}

HttpHelper::~HttpHelper() {

}

}
