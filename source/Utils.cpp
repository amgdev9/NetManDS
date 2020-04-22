/**
 * @file Utils.cpp
 * @brief Utility functions
 */

// Includes C/C++
#include <time.h>
#include <dirent.h>

// Own includes
#include "Utils.h"
#include "Application.h"

namespace NetMan {

/**
 * @brief Add a field to a JSON array
 * @param array The array to use
 * @param field The string to add
 */
void Utils::addJsonField(json_t *array, const std::string &field) {
    json_array_append_new(array, json_string(field.c_str()));
}

std::shared_ptr<json_t> Utils::loadJsonList(const std::string &path) {
    auto root = std::shared_ptr<json_t>(json_load_file(path.c_str(), 0, NULL), [=](json_t* data) { json_decref(data); });
    if(root.get() == NULL) {
        Application::getInstance().messageBox("Can't load JSON list: " + path);
    }
    return root;
}

/**
 * @brief Get current time
 * @return A string with the current time
 */
std::string Utils::getCurrentTime() {

    time_t unixTime = time(NULL);
	struct tm* timeStruct = gmtime((const time_t *)&unixTime);

	int hours = timeStruct->tm_hour;
	int minutes = timeStruct->tm_min;
	int seconds = timeStruct->tm_sec;

    char t[64];
    sprintf(t, "[%02i:%02i:%02i]", hours, minutes, seconds);
    return std::string(t);
}

/**
 * @brief Get a port from a formulary
 * @param params    EditText params
 * @param port      Initial port
 */
u16 Utils::handleFormPort(EditTextParams *params, u16 port) {
    u16 ret = 0;
    if(!params->init) {
        sprintf(params->text, "%d", port);
        params->init = true;
    } else {
        int port = strtol(params->text, NULL, 10);
        if(port == 0 || port > 0xFFFF) {
            params->init = false;
            Application::getInstance().messageBox("Invalid port");
        } else {
            ret = port;
        }
    }

    return ret;
}

/**
 * @brief Get an integer from a formulary
 * @param params    EditText params
 * @param dest      Destination field
 * @param limit     Integer max range
 */
void Utils::handleFormInteger(EditTextParams *params, u32 *dest, u32 limit) {
    if(!params->init) {
        sprintf(params->text, "%ld", *dest);
        params->init = true;
    } else {
        int n = strtol(params->text, NULL, 10);
        if(n == 0 || (u32)n > limit) {
            params->init = false;
            Application::getInstance().messageBox("Invalid number");
        } else {
            *dest = (u32)n;
        }
    }
}

/**
 * @brief Read a folder
 * @param path  Folder path
 * @param ext   Files extension
 * @param out   Out vector with folder entries
 */
void Utils::readFolder(const std::string &path, const std::string &ext, std::vector<std::string> &out) {

    DIR *dp = opendir(path.c_str());
    if(dp == NULL) {
        throw std::runtime_error("Couldn't open " + path);
    }

    struct dirent *entry;
    struct stat statbuf;
    while((entry = readdir(dp)) != NULL) {

        lstat(entry->d_name, &statbuf);

        if(!S_ISDIR(statbuf.st_mode)) {
            if(ext.length() > 0) {
                int dnameLen = strlen(entry->d_name);
                if(strcmp(ext.c_str(), &entry->d_name[dnameLen - ext.length()]) == 0) {
                    out.push_back(entry->d_name);
                }
            } else {
                out.push_back(entry->d_name);
            }
        }
    }

    closedir(dp);
}

}
