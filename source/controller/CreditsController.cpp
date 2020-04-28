/**
 * @file CreditsController.cpp
 */

// Own includes
#include "controller/CreditsController.h"
#include "Application.h"

namespace NetMan {

/**
 * @brief Go to the main menu
 */
static void gotoMenu(void *args) {
    Application::getInstance().requestLayoutChange("menu");
}

/**
 * @brief Constructor for a CreditsController
 */
CreditsController::CreditsController() {
    this->cbMap = std::unordered_map<std::string, void(*)(void*)> {
        {"gotoMenu", gotoMenu},
    };
}

/**
 * @brief Destructor for a CreditsController
 */
CreditsController::~CreditsController() { }

}
