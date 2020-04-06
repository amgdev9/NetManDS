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
#include "controller/MenuTopController.h"
#include "controller/SshController.h"

namespace NetMan {

template<typename T> static GuiController *bakeController() { return new T; }

// Map to store controllers
const static std::unordered_map<std::string, GuiController*(*)()> controllerFactory = {
    {"MainController", &bakeController<MainController>},
    {"MenuController", &bakeController<MenuController>},
    {"MenuTopController", &bakeController<MenuTopController>},
    {"SshController", &bakeController<SshController>},
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

/**
 * @brief Call a method from the controller
 * @param method    Method to call
 * @param args      Arguments to the method
 */
void GuiController::callMethod(const std::string &method, void *args) {
    auto it = cbMap.find(method);
    if(it != cbMap.end()) {
        it->second(args);
    }
}

}
