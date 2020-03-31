/**
 * @file YinHelper.h
 * @brief Helper for XML YANG files
 */
#ifndef _YIN_HELPER_H_
#define _YIN_HELPER_H_

// Includes C/C++
#include <string>

// Includes tinyxml2
#include <tinyxml2.h>

namespace NetMan {

/**
 * @class YinHelper
 */
class YinHelper {
    private:
        tinyxml2::XMLDocument doc;
    public:
        YinHelper(const std::string &path);
        virtual ~YinHelper();
};

}

#endif
