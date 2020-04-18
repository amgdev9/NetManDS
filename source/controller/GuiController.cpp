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
#include "controller/CreditsController.h"
#include "controller/OptionsController.h"
#include "controller/AddUserController.h"
#include "controller/SetSMIController.h"
#include "controller/SshConsoleController.h"

namespace NetMan {

template<typename T> static GuiController *bakeController() { return new T; }

// Map to store controllers
const static std::unordered_map<std::string, GuiController*(*)()> controllerFactory = {
    {"MainController", &bakeController<MainController>},
    {"MenuController", &bakeController<MenuController>},
    {"MenuTopController", &bakeController<MenuTopController>},
    {"SshController", &bakeController<SshController>},
    {"CreditsController", &bakeController<CreditsController>},
    {"OptionsController", &bakeController<OptionsController>},
    {"AddUserController", &bakeController<AddUserController>},
    {"SetSMIController", &bakeController<SetSMIController>},
    {"SshConsoleController", &bakeController<SshConsoleController>},
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

    try {
        return std::shared_ptr<GuiController>(it->second());
    } catch (const std::bad_alloc &e) {
        throw;
    } catch (const std::runtime_error &e) {
        throw;
    }
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
