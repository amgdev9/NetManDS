/**
 * @file SnmpController.cpp
 */

// Own includes
#include "controller/SnmpController.h"
#include "Application.h"

namespace NetMan {

static void goAgentDiscovery(void *args) {
    Application::getInstance().requestLayoutChange("agentdiscovery");
}

static void goMibBrowser(void *args) {

}

static void goSendSnmp(void *args) {

}

static void goMenu(void *args) {
    Application::getInstance().requestLayoutChange("menu");
}

SnmpController::SnmpController() {
    this->cbMap = std::unordered_map<std::string, void(*)(void*)> {
        {"goMenu", goMenu},
        {"goSendSnmp", goSendSnmp},
        {"goAgentDiscovery", goAgentDiscovery},
        {"goMibBrowser", goMibBrowser},
    };
}

SnmpController::~SnmpController() { }

}
