/**
 * @file SnmpController.cpp
 */

// Own includes
#include "controller/SnmpController.h"
#include "Application.h"

namespace NetMan {

/**
 * @brief Go to the Agent Discovery screen
 */
static void goAgentDiscovery(void *args) {
    Application::getInstance().requestLayoutChange("agentdiscovery");
}

/**
 * @brief Go to the MIB Browser screen
 */
static void goMibBrowser(void *args) {
    std::shared_ptr<u8> contextData = std::make_shared<u8>();
    Application::getInstance().requestLayoutChange("setsmi", contextData);
}

/**
 * @brief Go to the Send SNMP screen
 */
static void goSendSnmp(void *args) {
    Application::getInstance().requestLayoutChange("sendsnmp");
}

/**
 * @brief Go to the main menu
 */
static void goMenu(void *args) {
    Application::getInstance().requestLayoutChange("menu");
}

/**
 * @brief Constructor for a SnmpController
 */
SnmpController::SnmpController() {
    this->cbMap = std::unordered_map<std::string, void(*)(void*)> {
        {"goMenu", goMenu},
        {"goSendSnmp", goSendSnmp},
        {"goAgentDiscovery", goAgentDiscovery},
        {"goMibBrowser", goMibBrowser},
    };
}

/**
 * @brief Destructor for a SnmpController
 */
SnmpController::~SnmpController() { }

}
