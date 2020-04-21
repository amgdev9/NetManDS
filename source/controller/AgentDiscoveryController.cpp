/**
 * @file AgentDiscoveryController.cpp
 */

// Own includes
#include "controller/AgentDiscoveryController.h"
#include "gui/ButtonView.h"
#include "Application.h"
#include "snmp/SnmpAgentScanner.h"

namespace NetMan {

static void goSnmp(void *args) {
    Application::getInstance().requestLayoutChange("snmp");
}

static void editRange(void *args) {

}

static void editPort(void *args) {

}

static void editVersion(void *args) {

}

static void editMaxRequests(void *args) {

}

static void editTimeout(void *args) {

}

static void scan(void *args) {
    ButtonParams *params = (ButtonParams*)args;
    auto controller = std::static_pointer_cast<AgentDiscoveryController>(params->controller);
    auto &data = controller->getParams();

    if(data.baseIP == 0 || data.nhosts == 0 || data.maxRequests == 0 || data.timeout == 0) {
        Application::getInstance().messageBox("Invalid scan data");
    } else {
        auto contextData = std::make_shared<AgentDiscoveryParams>();
        memcpy(contextData.get(), &data, sizeof(AgentDiscoveryParams));
        Application::getInstance().requestLayoutChange("agentscanner", contextData);
    }
}

AgentDiscoveryController::AgentDiscoveryController() {
    this->cbMap = std::unordered_map<std::string, void(*)(void*)> {
        {"goSnmp", goSnmp},
        {"editRange", editRange},
        {"editPort", editPort},
        {"editVersion", editVersion},
        {"editMaxRequests", editMaxRequests},
        {"editTimeout", editTimeout},
        {"scan", scan},
    };

    auto contextData = Application::getInstance().getContextData();
    if(contextData != nullptr) {
        auto paramData = std::static_pointer_cast<AgentDiscoveryParams>(contextData);
        memcpy(&params, paramData.get(), sizeof(AgentDiscoveryParams));
    } else {
        memset(&params, 0, sizeof(AgentDiscoveryParams));
    }
}

AgentDiscoveryController::~AgentDiscoveryController() { }

}
