/**
 * @file Utils.h
 * @brief Utility functions
 */

#ifndef _UTILS_H_
#define _UTILS_H_

// Includes C/C++
#include <string>

// Includes jansson
#include <jansson.h>

// Own includes
#include "gui/EditTextView.h"
#include "controller/SnmpParamsController.h"
#include "controller/RestConfOpController.h"

namespace NetMan {

/**
 * @class Utils
 */
class Utils {
    public:
        static void addJsonField(json_t *array, const std::string &field);
        static std::string getCurrentTime();
        static u16 handleFormPort(EditTextParams *params, u16 port);
        static void handleFormInteger(EditTextParams *params, u32 *dest, u32 limit);
        static void readFolder(const std::string &path, const std::string &ext, std::vector<std::string> &out);
        static std::shared_ptr<json_t> loadJsonList(const std::string &path);
        static bool endsWith(const std::string &mainStr, const std::string &toMatch);
        static void sendSnmpPdu(SnmpThreadParams *params);
        static void sendRestConf(std::shared_ptr<RestConfParams> params);
};

}

#endif
