/**
 * @file RestConfClient.cpp
 * @brief RestConf Client
 */

// Includes C/C++
#include <stdexcept>

// Includes 3DS
#include <3ds.h>

// Includes tinyxml2
#include <tinyxml2.h>

// Own includes
#include "restconf/RestConfClient.h"

using namespace tinyxml2;

// Base64 characters
static const std::string base64_chars = 
             "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
             "abcdefghijklmnopqrstuvwxyz"
             "0123456789+/";

namespace NetMan {

/**
 * @brief Constructor for a RESTCONF Client
 * @param host          Host to access
 * @param username      User name
 * @param password      Password for basic authentication
 * @param timeoutSecs   Timeout for receiving HTTP responses, in seconds
 */
RestConfClient::RestConfClient(const std::string &host, const std::string &username, const std::string &password, u32 timeoutSecs) {

    // Initialize data
    this->username = username;
    this->password = password;
    this->host = host;
    this->timeoutSecs = timeoutSecs;
    this->statusCode = 0;

    // Discover the server
    std::shared_ptr<u8> xmlDiscovery = httpRequest(host + "/.well-known/host-meta", HTTPC_METHOD_GET);

    // Decode the retrieved XML
    XMLDocument doc;
    XMLError res = doc.Parse((char*)xmlDiscovery.get());
    if(res != XML_SUCCESS) {
        throw std::runtime_error("Invalid restconf server: no /.well-known/host-meta");
    }

    XMLNode *elem = doc.RootElement()->FirstChild();
    if(elem == NULL) {
        throw std::runtime_error("Invalid host-meta");
    }

    const char *href = elem->ToElement()->Attribute("href");
    if(href == NULL) {
        throw std::runtime_error("No href found");
    }

    // Save the decoded rootAlias
    rootAlias = std::string(href);
}

/**
 * @brief Perform an HTTP request
 * @param url           Complete URL
 * @param method        HTTP method
 * @param postData      Data for POST requests
 * @param postDataSize  Size of POST data
 * @return The response body, or nullptr if it is empty
 */
std::shared_ptr<u8> RestConfClient::httpRequest(const std::string &url, HTTPC_RequestMethod method, std::shared_ptr<u8> postData, u32 postDataSize) {

    httpcContext context;
    if(httpcOpenContext(&context, method, url.c_str(), 0) < 0) {
        throw std::runtime_error("httpcOpenContext");
    }

	if(httpcSetSSLOpt(&context, SSLCOPT_DisableVerify) < 0) {
        throw std::runtime_error("httpcSetSSLOpt");
    }

	if(httpcSetKeepAlive(&context, HTTPC_KEEPALIVE_ENABLED) < 0) {
        throw std::runtime_error("httpcSetKeepAlive");
    }
	
    std::string userPass = username + ":" + password;
    httpcAddRequestHeaderField(&context, "Authorization", std::string("Basic " + base64_encode((u8*)userPass.c_str(), userPass.length())).c_str());
    httpcAddRequestHeaderField(&context, "Accept", "application/yang-data+json");

    if(method == HTTPC_METHOD_POST && postData != nullptr && postDataSize > 0) {
        httpcAddRequestHeaderField(&context, "Content-Type", "application/yang-data+json");
        httpcAddPostDataRaw(&context, (u32*)postData.get(), postDataSize);
    }

    if(httpcBeginRequest(&context) != 0) {
        httpcCloseContext(&context);
        throw std::runtime_error("httpcBeginRequest");
    }

    if(httpcGetResponseStatusCode(&context, &statusCode) != 0) {
        httpcCloseContext(&context);
        throw std::runtime_error("httpcGetResponseStatusCode");
    }

    if(statusCode != 200 && statusCode != 201 && statusCode != 204) {
        httpcCloseContext(&context);
        throw std::runtime_error("Error " + std::to_string(statusCode));
    }

    // POST and DELETE methods don't have a body in RESTCONF
    std::shared_ptr<u8> buf = nullptr;
    if(method != HTTPC_METHOD_POST && method != HTTPC_METHOD_DELETE) {
        try {
            buf = std::shared_ptr<u8>(new u8[RESTCONF_HTTP_BUFFER_SIZE]);
            memset(buf.get(), 0, RESTCONF_HTTP_BUFFER_SIZE);
        } catch (const std::bad_alloc &e) {
            throw;
        }

        if(httpcReceiveDataTimeout(&context, buf.get(), RESTCONF_HTTP_BUFFER_SIZE, timeoutSecs * 1000000000L) < 0) {
            httpcCloseContext(&context);
            throw std::runtime_error("httpcReceiveData");
        }
    }

    httpcCloseContext(&context);

    return buf;
}

/**
 * @brief Perform a RESTCONF HTTP request to a server
 * @param url       URL chunk, obtained from YinHelper methods
 * @param method    HTTP Method
 * @param postData  JSON to use for POST data
 * @return The response JSON, or nullptr if no response is available
 */
std::shared_ptr<json_t> RestConfClient::request(const std::string &url, HTTPC_RequestMethod method, std::shared_ptr<json_t> postData) {
    try {

        // Serializa JSON data, if needed
        std::shared_ptr<u8> encodedData = nullptr;
        u32 encodedDataSize = 0;
        if(method == HTTPC_METHOD_POST && postData != NULL) {
            encodedData = std::shared_ptr<u8>((u8*)json_dumps(postData.get(), 0));
            encodedDataSize = strlen((char*)encodedData.get());
        }

        // Perform the request
        auto ptr = httpRequest(host + rootAlias + url, method, encodedData, encodedDataSize);
        if(ptr != nullptr) {
            json_error_t jsonError;
            json_t *jsonData = json_loads((char*)ptr.get(), 0, &jsonError);
            if(jsonData == NULL) {
                throw std::runtime_error("Error loading JSON");
            }
            return std::shared_ptr<json_t>(jsonData, [=](json_t* data) { json_decref(data); });
        }

        return nullptr;
    } catch (const std::bad_alloc &e) {
        throw;
    } catch (const std::runtime_error &e) {
        throw;
    }
}

/**
 * @brief Destructor for a RESTCONF Client
 */
RestConfClient::~RestConfClient() { }

/**
 * @brief Encode a bunch of bytes to base64
 * @param bytes_to_encode   Buffer to encode
 * @param in_len            Buffer length
 * @return A string with the base64 form of the input buffer
 * @note Obtained from https://renenyffenegger.ch/notes/development/Base64/Encoding-and-decoding-base-64-with-cpp
 */
std::string RestConfClient::base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len) {
    
    std::string ret;
    int i = 0;
    int j = 0;
    unsigned char char_array_3[3];
    unsigned char char_array_4[4];

    while (in_len--) {
        char_array_3[i++] = *(bytes_to_encode++);
        if (i == 3) {
        char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
        char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
        char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
        char_array_4[3] = char_array_3[2] & 0x3f;

        for(i = 0; (i <4) ; i++)
            ret += base64_chars[char_array_4[i]];
        i = 0;
        }
    }

    if (i)
    {
        for(j = i; j < 3; j++)
        char_array_3[j] = '\0';

        char_array_4[0] = ( char_array_3[0] & 0xfc) >> 2;
        char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
        char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);

        for (j = 0; (j < i + 1); j++)
        ret += base64_chars[char_array_4[j]];

        while((i++ < 3))
        ret += '=';

    }

    return ret;
}

}
