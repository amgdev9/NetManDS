/**
 * @file GuiController.cpp
 * @brief GUI Controller interface
 */

// Includes C/C++
#include <unordered_map>

// Own includes
#include "controller/GuiController.h"
#include "controller/MainController.h"
#include "controller/MenuController.h"

namespace NetMan {

template<typename T> static GuiController *bakeController() { return new T; }

// Map to store controllers
const static std::unordered_map<std::string, GuiController*(*)()> controllerFactory = {
    {"MainController", &bakeController<MainController>},
    {"MenuController", &bakeController<MenuController>},
};

/**
 * @brief Create a controller instance
 * @param className     Controller class name
 * @return The newly created controller
 */
std::shared_ptr<GuiController> GuiController::createController(const std::string &className) {
    auto it = controllerFactory.find(className);
    if(it == controllerFactory.end()) {
        throw std::runtime_error("Controller " + className + " not found");
    }

    return std::shared_ptr<GuiController>(it->second());
}

}
