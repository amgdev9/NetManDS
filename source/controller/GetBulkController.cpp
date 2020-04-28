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

/**
 * @brief Go to the Send SNMP screen
 */
static void goBack(void *args) {
    Application::getInstance().requestLayoutChange("sendsnmp");
}

/**
 * @brief Edit non-repeaters field
 */
static void editNonRepeaters(void *args) {
    EditTextParams *params = (EditTextParams*)args;
    auto controller = std::static_pointer_cast<GetBulkController>(params->controller);
    Utils::handleFormInteger(params, &controller->getSessionParams()->nonRepeaters, 999);
}

/**
 * @brief Edit max-repetitions field
 */
static void editMaxRepetitions(void *args) {
    EditTextParams *params = (EditTextParams*)args;
    auto controller = std::static_pointer_cast<GetBulkController>(params->controller);
    Utils::handleFormInteger(params, &controller->getSessionParams()->maxRepetitions, 999);
}

/**
 * @brief Called when send button is clicked
 */
static void sendSnmp(void *args) {
    ButtonParams *params = (ButtonParams*)args;
    auto controller = std::static_pointer_cast<GetBulkController>(params->controller);
    Application::getInstance().requestLayoutChange("agentview", controller->getSessionParams());
}

/**
 * @brief Constructor for a GetBulkController
 */
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

    // Initialize data
    this->sessionParams = contextData;
    this->sessionParams->nonRepeaters = 0;
    this->sessionParams->maxRepetitions = 0;
}

/**
 * @brief Destructor for a GetBulkController
 */
GetBulkController::~GetBulkController() { }

}
