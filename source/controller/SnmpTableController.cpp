/**
 * @file SnmpTableController.cpp
 */

// Own includes
#include "controller/SnmpTableController.h"
#include "Application.h"

namespace NetMan {

static void gotoMenu(void *args) {
    Application::getInstance().requestLayoutChange("menu");
}

SnmpTableController::SnmpTableController() {
    this->cbMap = std::unordered_map<std::string, void(*)(void*)> {
        {"gotoMenu", gotoMenu},
    };
}

SnmpTableController::~SnmpTableController() { }

}
