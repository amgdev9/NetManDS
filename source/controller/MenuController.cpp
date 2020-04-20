/**
 * @file MenuController.cpp
 */

// Includes C/C++
#include <arpa/inet.h>

// Includes 3DS
#include <3ds/types.h>

// Own includes
#include "controller/MenuController.h"
#include "Config.h"
#include "Application.h"
#include "syslog/SyslogPdu.h"

namespace NetMan {

static void goSNMP(void *args) {

}

static void goRestConf(void *args) {

}

static void goLogs(void *args) {
    Application::getInstance().requestLayoutChange("logs");
}

static void goSSH(void *args) {
    Application::getInstance().requestLayoutChange("ssh");
}

static void goCredits(void *args) {
    Application::getInstance().requestLayoutChange("credits");
}

static void goOptions(void *args) {
    Application::getInstance().requestLayoutChange("options");
}

MenuController::MenuController() {
    this->cbMap = std::unordered_map<std::string, void(*)(void*)> {
        {"goSSH", goSSH},
        {"goCredits", goCredits},
        {"goOptions", goOptions},
        {"goSNMP", goSNMP},
        {"goRestConf", goRestConf},
        {"goLogs", goLogs},
    };
}

MenuController::~MenuController() { }

}
