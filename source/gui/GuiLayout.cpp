/**
 * @file GuiLayout.cpp
 * @brief GUI layout
 */

// Includes C/C++
#include <stdexcept>

// Own includes
#include "gui/GuiLayout.h"
#include "gui/ImageView.h"
#include "gui/ButtonView.h"
#include "gui/TextView.h"

// Includes tinyxml2
#include <tinyxml2.h>

using namespace tinyxml2;

namespace NetMan {

// Auxiliar bakers
template<typename T> static GuiView *bakeView(XMLElement *node, std::shared_ptr<GuiController> controller) { return new T(node, controller); }

// Map to emulate reflection on cpp
const static std::unordered_map<std::string, GuiView*(*)(XMLElement*, std::shared_ptr<GuiController>)> viewFactory = {
    {"ImageView", &bakeView<ImageView>},
    {"ButtonView", &bakeView<ButtonView>},
    {"TextView", &bakeView<TextView>},
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

    // Create the controller, if any
    controller = nullptr;
    const char *controllerClass = root->Attribute("controller");
    if(controllerClass != NULL) {
        try {
            controller = GuiController::createController(controllerClass);
        } catch (const std::bad_alloc &e) {
            throw;
        } catch (const std::runtime_error &e) {
            throw;
        }
    }

    // Initialize views vector
    views = std::vector<std::shared_ptr<GuiView>>();

    // Load every view
    for(XMLElement* child = root->FirstChildElement(); child != NULL; child = child->NextSiblingElement()) {
        auto view = viewFactory.find(child->Name());
        if(view != viewFactory.end()) {
            try {
                views.push_back(std::shared_ptr<GuiView>(view->second(child, controller)));
            } catch (const std::bad_alloc &e) {
                throw;
            } catch (const std::runtime_error &e) {
                throw;
            }
        } else {
            throw std::runtime_error(std::string("Not found: ") + child->Name());
        }
    }

    // Call the controller initializer, if any
    if(controller != nullptr) {
        controller->initialize(views);
    }
}

/**
 * @brief Process input
 * @param held  Held keys
 * @param down  Down keys
 * @param up    Up keys
 * @param touch Touchpad state
 */
void GuiLayout::input(u32 held, u32 down, u32 up, touchPosition &touch) {
    for(auto& view : views) {
        view->input(held, down, up, touch);
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
