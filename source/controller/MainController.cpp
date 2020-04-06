/**
 * @file MainController.cpp
 */

// Own includes
#include "controller/MainController.h"
#include "Application.h"

namespace NetMan {

static void gotoMenu(void *args) {
    Application::getInstance().requestLayoutChange("menu");
}

MainController::MainController() {
    this->cbMap = std::unordered_map<std::string, void(*)(void*)> {
        {"gotoMenu", gotoMenu},
    };
}

MainController::~MainController() { }

}
