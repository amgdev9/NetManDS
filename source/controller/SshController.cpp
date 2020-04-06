/**
 * @file SshController.cpp
 */

// Own includes
#include "controller/SshController.h"
#include "Application.h"

namespace NetMan {

static void goMenu(void *args) {
    Application::getInstance().requestLayoutChange("menu");
}

// Callback map
const static std::unordered_map<std::string, void(*)(void*)> cbMap = {
    {"goMenu", goMenu},
};

SshController::SshController() {
    this->cbMap = std::unordered_map<std::string, void(*)(void*)> {
        {"goMenu", goMenu},
    };
}

SshController::~SshController() { }

}
