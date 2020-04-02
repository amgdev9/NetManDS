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

/**
 * @brief Constructor for a YIN Helper
 * @param path  Path of the .yin file
 */
YinHelper::YinHelper(const std::string &path) {

    // Load the YIN file as XML
    XMLError res = doc.LoadFile(path.c_str());
    if(res != XML_SUCCESS) {
        throw std::runtime_error(std::string("Couldn't load ") + path);
    }

    // Get the root node
    root = doc.RootElement();
}

/**
 * @brief Generate a RESTCONF URL from a container node
 * @param node  Node to be used
 * @return The generated RESTCONF URL
 */
std::string YinHelper::getRestConfContainerURL(tinyxml2::XMLElement *node) {

    if(strcmp(node->Name(), "container") != 0) {
        throw std::runtime_error("No container found");
    }

    const char *containerName = node->Attribute("name");
    if(containerName == NULL) {
        throw std::runtime_error("No container name found");
    }

    const char *moduleName = root->Attribute("name");
    if(moduleName == NULL || node->Parent() != root) {
        throw std::runtime_error("No module found");
    }

    return std::string(std::string("/data/") + moduleName + ":" + containerName);
}

/**
 * @brief Generate a RESTCONF URL from a leaf or list node
 * @param node  Node to be used
 * @return The generated RESTCONF URL
 */
std::string YinHelper::getRestConfURL(XMLElement *node, const std::string &listKey, const std::vector<std::string> &fields, const std::string &content) {
    
    bool isLeaf = strcmp(node->Name(), "leaf") == 0;
    bool isList = strcmp(node->Name(), "list") == 0;
    
    if(!isLeaf && !isList) {
        throw std::runtime_error("Not a leaf or list");
    }

    const char *nodeName = node->Attribute("name");
    if(nodeName == NULL) {
        throw std::runtime_error("No leaf/list name found");
    }

    XMLElement *container = node->Parent()->ToElement();
    if(strcmp(node->Parent()->ToElement()->Name(), "container") != 0) {
        throw std::runtime_error("No container found");
    }
    const char *containerName = container->Attribute("name");
    if(containerName == NULL) {
        throw std::runtime_error("No container name found");
    }

    const char *moduleName = root->Attribute("name");
    if(moduleName == NULL || container->Parent() != root) {
        throw std::runtime_error("No module found");
    }

    std::string url = std::string(std::string("/data/") + moduleName + ":" + containerName + "/" + nodeName);

    if(isList && !listKey.empty()) {
        url.append("=" + listKey);
    }

    url.append("?content=" + content);

    if(isList && fields.size() > 0) {
        for(auto field : fields) {
            url.append("&fields=" + field);
        }
    }

    return url;
}

/**
 * @brief Get the fields of a list element
 * @param node      List node
 * @param fields    Vector to store the fields
 */
void YinHelper::getListFields(XMLElement *node, std::vector<std::string> &fields) {

    if(strcmp(node->Name(), "list") != 0) {
        throw std::runtime_error("Not a list");
    }

    for(XMLElement* child = node->FirstChildElement(); child != NULL; child = child->NextSiblingElement()) {
        if(strcmp(child->Name(), "leaf") == 0) {
            fields.push_back(child->Attribute("name"));
        }
    }
}

/**
 * @brief Destructor for a YIN Helper
 */
YinHelper::~YinHelper() { }

}
