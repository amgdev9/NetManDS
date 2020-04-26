/**
 * @file GetBulkController.cpp
 */

// Own includes
#include "controller/GetBulkController.h"
#include "gui/ButtonView.h"
#include "gui/EditTextView.h"
#include "Application.h"
#include "Utils.h"

namespace NetMan {

static void goBack(void *args) {
    Application::getInstance().requestLayoutChange("sendsnmp");
}

static void editNonRepeaters(void *args) {
    EditTextParams *params = (EditTextParams*)args;
    auto controller = std::static_pointer_cast<GetBulkController>(params->controller);
    Utils::handleFormInteger(params, &controller->getSessionParams()->nonRepeaters, 999);
}

static void editMaxRepetitions(void *args) {
    EditTextParams *params = (EditTextParams*)args;
    auto controller = std::static_pointer_cast<GetBulkController>(params->controller);
    Utils::handleFormInteger(params, &controller->getSessionParams()->maxRepetitions, 999);
}

static void sendSnmp(void *args) {
    ButtonParams *params = (ButtonParams*)args;
    auto controller = std::static_pointer_cast<GetBulkController>(params->controller);
    Application::getInstance().requestLayoutChange("agentview", controller->getSessionParams());
}

GetBulkController::GetBulkController() {
    this->cbMap = std::unordered_map<std::string, void(*)(void*)> {
        {"goBack", goBack},
        {"editNonRepeaters", editNonRepeaters},
        {"editMaxRepetitions", editMaxRepetitions},
        {"sendSnmp", sendSnmp},
    };

    auto contextData = std::static_pointer_cast<SnmpSessionParams>(Application::getInstance().getContextData());
    if(contextData == nullptr) {
        throw std::runtime_error("No context specified");
    }
    this->sessionParams = contextData;
    this->sessionParams->nonRepeaters = 0;
    this->sessionParams->maxRepetitions = 0;
}

GetBulkController::~GetBulkController() { }

}
