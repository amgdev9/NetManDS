/**
 * @file RestConfClient.h
 * @brief RestConf Client
 */
#ifndef _RESTCONF_CLIENT_H_
#define _RESTCONF_CLIENT_H_

// Includes C/C++
#include <string>
#include <memory>

// Includes jansson
#include <jansson.h>

// Includes 3DS
#include <3ds/types.h>

// Defines
#define RESTCONF_HTTP_BUFFER_SIZE   (256 << 10)

namespace NetMan {

/**
 * @class RestConfClient
 */
class RestConfClient {
    private:
        std::string host;
        std::string username;
        std::string password;
        std::string rootAlias;
        u32 timeoutSecs;
        u32 statusCode;
        std::string base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len);
        std::shared_ptr<u8> httpRequest(const std::string &url, HTTPC_RequestMethod method, std::shared_ptr<u8> postData = nullptr, u32 postDataSize = 0);
    public:
        RestConfClient(const std::string &host, const std::string &username, const std::string &password, u32 timeoutSecs);
        std::shared_ptr<json_t> request(const std::string &url, HTTPC_RequestMethod method, std::shared_ptr<json_t> postData = nullptr);
        inline u32 getStatusCode() { return statusCode; }
        virtual ~RestConfClient();
};

}

#endif
