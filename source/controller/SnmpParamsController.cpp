/**
 * @file SnmpParamsController.cpp
 */

// Own includes
#include "controller/SnmpParamsController.h"
#include "gui/BinaryButtonView.h"
#include "gui/EditTextView.h"
#include "Application.h"

namespace NetMan {

static void editSnmpVersion(void *args) {
    BinaryButtonParams *params = (BinaryButtonParams*)args;
    auto controller = std::static_pointer_cast<SnmpParamsController>(params->controller);
    if(!params->init) return;
    controller->setUsmEnabled(params->selected);
}

static void editCommunity(void *args) {
    EditTextParams *params = (EditTextParams*)args;
    auto controller = std::static_pointer_cast<SnmpParamsController>(params->controller);
    if(!params->init) return;
    controller->setCommunity(params->text);
}

static void editUsername(void *args) {
    EditTextParams *params = (EditTextParams*)args;
    auto controller = std::static_pointer_cast<SnmpParamsController>(params->controller);
    if(!params->init) return;
    controller->setUsername(params->text);
}

static void sendSnmp(void *args) {
    // TODO
}

static void goBack(void *args) {
    Application::getInstance().requestLayoutChange("snmp");
}

SnmpParamsController::SnmpParamsController() {

    this->cbMap = std::unordered_map<std::string, void(*)(void*)> {
        {"editSnmpVersion", editSnmpVersion},
        {"editCommunity", editCommunity},
        {"editUsername", editUsername},
        {"sendSnmp", sendSnmp},
        {"goBack", goBack},
    };

    this->usmEnabled = false;
    this->community = "";
    this->username = "";
}

SnmpParamsController::~SnmpParamsController() { }

}
