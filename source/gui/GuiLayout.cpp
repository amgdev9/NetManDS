/**
 * @file GuiLayout.cpp
 * @brief GUI layout
 */

// Includes C/C++
#include <stdexcept>
#include <unordered_map>

// Own includes
#include "gui/GuiLayout.h"
#include "gui/ImageView.h"

// Includes tinyxml2
#include <tinyxml2.h>

using namespace tinyxml2;

namespace NetMan {

// Auxiliar view baker
template<typename T> GuiView *bakeView(XMLElement *node) { return new T(node); }

// Map to emulate reflection on cpp
const static std::unordered_map<std::string, GuiView*(*)(XMLElement*)> viewFactory = {
    {"ImageView", &bakeView<ImageView>},
};

/**
 * @brief Load a XML layout
 * @param path  Path for the XML layout file (without extension, lookup directory is romfs:/layout/)
 */
GuiLayout::GuiLayout(const std::string &path) {

    // Load the layout
    XMLDocument doc;
    XMLError res = doc.LoadFile(std::string("romfs:/layout/" + path + ".xml").c_str());
    if(res != XML_SUCCESS) {
        throw std::runtime_error(std::string("Couldn't load ") + path);
    }

    // Get the root element
    XMLElement *root = doc.RootElement();

    // Initialize views vector
    views = std::vector<std::shared_ptr<GuiView>>();

    // Load every view
    for(XMLElement* child = root->FirstChildElement(); child != NULL; child = child->NextSiblingElement()) {
        auto view = viewFactory.find(child->Name());
        if(view != viewFactory.end()) {
            try {
                views.push_back(std::shared_ptr<GuiView>(view->second(child)));
            } catch (const std::bad_alloc &e) {
                throw;
            } catch (const std::runtime_error &e) {
                throw;
            }
        } else {
            throw std::runtime_error(std::string("Not found: ") + child->Name());
        }
    }
}

/**
 * @brief Draw a layout
 */
void GuiLayout::draw() {
    for(auto& view : views) {
        view->draw();
    }
}

/**
 * @brief Destructor for a GuiLayout
 */
GuiLayout::~GuiLayout() { }

}
