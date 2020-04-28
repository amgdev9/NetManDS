/**
 * @file MainController.cpp
 */

// Own includes
#include "controller/MainController.h"
#include "Application.h"

namespace NetMan {

/**
 * @brief Go to the main menu
 */
static void gotoMenu(void *args) {
    Application::getInstance().requestLayoutChange("menu");
}

/**
 * @brief Constructor for a MainController
 */
MainController::MainController() {
    this->cbMap = std::unordered_map<std::string, void(*)(void*)> {
        {"gotoMenu", gotoMenu},
    };
}

/**
 * @brief Destructor for a MainController
 */
MainController::~MainController() { }

}
