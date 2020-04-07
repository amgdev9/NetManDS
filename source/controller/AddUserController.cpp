/**
 * @file AddUserController.cpp
 */

// Own includes
#include "controller/AddUserController.h"
#include "Application.h"

namespace NetMan {

static void gotoOptions(void *args) {
    Application::getInstance().requestLayoutChange("options");
}

AddUserController::AddUserController() {
    this->cbMap = std::unordered_map<std::string, void(*)(void*)> {
        {"gotoOptions", gotoOptions},
    };
}

AddUserController::~AddUserController() { }

}
