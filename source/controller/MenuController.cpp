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
#include "gui/TextView.h"

// Defines
#define MENUTEXT_X      140

namespace NetMan {

MenuController::MenuController() { }

MenuController::~MenuController() { }

void MenuController::callMethod(const std::string &method, void *args) {
    
}

}
