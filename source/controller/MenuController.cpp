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

/**
 * @brief Go to the SNMP menu
 */
static void goSNMP(void *args) {
    Application::getInstance().requestLayoutChange("snmp");
}

/**
 * @brief Go to the RESTCONF menu
 */
static void goRestConf(void *args) {
    Application::getInstance().requestLayoutChange("loadyin");
}

/**
 * @brief Go to the logs screen
 */
static void goLogs(void *args) {
    Application::getInstance().requestLayoutChange("logs");
}

/**
 * @brief Go to the SSH menu
 */
static void goSSH(void *args) {
    Application::getInstance().requestLayoutChange("ssh");
}

/**
 * @brief Go to the credits screen
 */
static void goCredits(void *args) {
    Application::getInstance().requestLayoutChange("credits");
}

/**
 * @brief Go to the options screen
 */
static void goOptions(void *args) {
    Application::getInstance().requestLayoutChange("options");
}

/**
 * @brief Constructor for a MenuController
 */
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

/**
 * @brief Destructor for a MenuController
 */
MenuController::~MenuController() { }

}
