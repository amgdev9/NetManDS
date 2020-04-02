/**
 * @file YinHelper.h
 * @brief Helper for XML YANG files
 */
#ifndef _YIN_HELPER_H_
#define _YIN_HELPER_H_

// Includes C/C++
#include <string>
#include <vector>

// Includes 3DS
#include <3ds/types.h>

// Includes tinyxml2
#include <tinyxml2.h>

namespace NetMan {

/**
 * @class YinHelper
 */
class YinHelper {
    private:
        tinyxml2::XMLDocument doc;
        tinyxml2::XMLElement *root;
    public:
        YinHelper(const std::string &path);
        inline tinyxml2::XMLElement *getRoot() { return root; }
        std::string getRestConfURL(tinyxml2::XMLElement *node, const std::string &key = "", const std::vector<std::string> &fields = std::vector<std::string>(), const std::string &content = "all");
        std::string getRestConfContainerURL(tinyxml2::XMLElement *node);
        void getListFields(tinyxml2::XMLElement *node, std::vector<std::string> &fields);
        virtual ~YinHelper();
};

}

#endif
