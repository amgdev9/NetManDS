/**
 * @file YinHelper.cpp
 * @brief Helper for XML YANG files
 */

// Includes C/C++
#include <stdexcept>

// Own includes
#include "restconf/YinHelper.h"

using namespace tinyxml2;

namespace NetMan {

YinHelper::YinHelper(const std::string &path) {

    XMLError res = doc.LoadFile(path.c_str());
    if(res != XML_SUCCESS) {
        throw std::runtime_error(std::string("Couldn't load ") + path);
    }
}

YinHelper::~YinHelper() {

}

}
