/**
 * @file CreditsController.cpp
 */

// Own includes
#include "controller/CreditsController.h"
#include "Application.h"

namespace NetMan {

static void gotoMenu(void *args) {
    Application::getInstance().requestLayoutChange("menu");
}

CreditsController::CreditsController() {
    this->cbMap = std::unordered_map<std::string, void(*)(void*)> {
        {"gotoMenu", gotoMenu},
    };
}

CreditsController::~CreditsController() { }

}
