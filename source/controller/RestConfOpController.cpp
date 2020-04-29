/**
 * @file RestConfOpController.cpp
 */

// Own includes
#include "controller/RestConfOpController.h"
#include "gui/ButtonView.h"
#include "Application.h"
#include "restconf/RestConfClient.h"
#include "Config.h"
#include "Utils.h"

// Defines
#define MAX_JSON_LENGTH     (8 << 10)

namespace NetMan {

/**
 * @brief Go to the Load YIN screen
 */
static void goBack(void *args) {
    Application::getInstance().requestLayoutChange("loadyin");
}

/**
 * @brief Perform a RESTCONF GET request
 */
static void getRequest(void *args) {
    ButtonParams *params = (ButtonParams*)args;
    auto controller = std::static_pointer_cast<RestConfOpController>(params->controller);
    auto rcParams = controller->getRestConfParams();
    rcParams->method = HTTPC_METHOD_GET;

    if(rcParams->isList) {
        Application::getInstance().requestLayoutChange("restconflist", rcParams);
    } else {
        try {
            Utils::sendRestConf(rcParams);
        } catch (const std::runtime_error &e) {
            Application::getInstance().messageBox(e.what());
        }
    }
}

/**
 * @brief Perform a RESTCONF POST request
 */
static void postRequest(void *args) {
    ButtonParams *params = (ButtonParams*)args;
    auto controller = std::static_pointer_cast<RestConfOpController>(params->controller);
    auto rcParams = controller->getRestConfParams();

    // Allocate data for post request
    char *postData = new char[MAX_JSON_LENGTH];
    if(postData == NULL) {
        Application::getInstance().messageBox("Can't allocate post buffer");
        return;
    }

    // Request post data
    SwkbdState swkbd;
    swkbdInit(&swkbd, SWKBD_TYPE_WESTERN, 1, -1);
    swkbdSetValidation(&swkbd, SWKBD_NOTEMPTY_NOTBLANK, 0, 0);
    swkbdSetFeatures(&swkbd, 0);
    swkbdSetHintText(&swkbd, "Enter JSON object:");
    swkbdSetFeatures(&swkbd, SWKBD_MULTILINE);
    swkbdInputText(&swkbd, postData, MAX_JSON_LENGTH);

    // Configure request
    rcParams->method = HTTPC_METHOD_POST;
    rcParams->postData = postData;

    // Send RESTCONF request
    try {
        Utils::sendRestConf(rcParams);
        Application::getInstance().messageBox("Data added successfully");
    } catch (const std::runtime_error &e) {
        Application::getInstance().messageBox(e.what());
    }
}

/**
 * @brief Perform a RESTCONF DELETE request
 */
static void deleteRequest(void *args) {
    ButtonParams *params = (ButtonParams*)args;
    auto controller = std::static_pointer_cast<RestConfOpController>(params->controller);
    auto rcParams = controller->getRestConfParams();

    if(rcParams->isList) {
        rcParams->method = HTTPC_METHOD_DELETE;
        Application::getInstance().requestLayoutChange("restconfkey", rcParams);
    } else {
        Application::getInstance().messageBox("Only list elements can be deleted");
    }
}

/**
 * @brief Constructor for a RestConfOpController
 */
RestConfOpController::RestConfOpController() {
    this->cbMap = std::unordered_map<std::string, void(*)(void*)> {
        {"goBack", goBack},
        {"getRequest", getRequest},
        {"postRequest", postRequest},
        {"deleteRequest", deleteRequest},
    };

    // Retrieve context data
    restConfParams = std::static_pointer_cast<RestConfParams>(Application::getInstance().getContextData());
    if(restConfParams == nullptr) {
        throw std::runtime_error("No context specified");
    }
}

/**
 * @brief Destructor for a RestConfOpController
 */
RestConfOpController::~RestConfOpController() { }

}
