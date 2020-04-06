/**
 * @file MenuController.cpp
 */

// Includes C/C++
#include <arpa/inet.h>

// Includes 3DS
#include <3ds/types.h>

// Own includes
#include "controller/MenuController.h"
#include "Application.h"

// Defines
#define MENUTEXT_X      140

namespace NetMan {

static void goSSH(void *args) {
    Application::getInstance().requestLayoutChange("ssh");
}

MenuController::MenuController() {
    this->cbMap = std::unordered_map<std::string, void(*)(void*)> {
        {"goSSH", goSSH},
    };
}

MenuController::~MenuController() { }

}
