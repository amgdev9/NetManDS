/**
 * @file RestConfKeyController.cpp
 */

// Own includes
#include "controller/RestConfKeyController.h"
#include "gui/ButtonView.h"
#include "gui/EditTextView.h"
#include "Application.h"
#include "Utils.h"

namespace NetMan {

/**
 * @brief Go to the RESTCONF operations screen
 */
static void goBack(void *args) {
    ButtonParams *params = (ButtonParams*)args;
    auto controller = std::static_pointer_cast<RestConfKeyController>(params->controller);
    auto rcParams = controller->getRestConfParams();
    rcParams->fields.clear();
    rcParams->key.clear();
    rcParams->content = "all";
    Application::getInstance().requestLayoutChange("restconfop", rcParams);
}

/**
 * @brief Edit the list key
 */
static void editKey(void *args) {
    EditTextParams *params = (EditTextParams*)args;
    auto controller = std::static_pointer_cast<RestConfKeyController>(params->controller);
    if(!params->init) {
        sprintf(params->text, controller->getRestConfParams()->key.c_str());
        params->init = true;
    } else {
        controller->getRestConfParams()->key.assign(params->text);
    }
}

/**
 * @brief Edit the content parameter
 */
static void editContent(void *args) {
    EditTextParams *params = (EditTextParams*)args;
    auto controller = std::static_pointer_cast<RestConfKeyController>(params->controller);
    if(!params->init) {
        sprintf(params->text, controller->getRestConfParams()->content.c_str());
        params->init = true;
    } else {
        controller->getRestConfParams()->content.assign(params->text);
    }
}

/**
 * @brief Send the RESTCONF request
 */
static void sendRestConf(void *args) {
    ButtonParams *params = (ButtonParams*)args;
    auto controller = std::static_pointer_cast<RestConfKeyController>(params->controller);
    try {
        Utils::sendRestConf(controller->getRestConfParams());

        if(controller->getRestConfParams()->method == HTTPC_METHOD_DELETE) {
            Application::getInstance().messageBox("Element deleted successfully");
        }
    } catch (const std::runtime_error &e) {
        Application::getInstance().messageBox(e.what());
    }
}

/**
 * @brief Constructor for a RestConfKeyController
 */
RestConfKeyController::RestConfKeyController() {
    this->cbMap = std::unordered_map<std::string, void(*)(void*)> {
        {"goBack", goBack},
        {"editKey", editKey},
        {"editContent", editContent},
        {"sendRestConf", sendRestConf},
    };
    
    // Retrieve context data
    rcParams = std::static_pointer_cast<RestConfParams>(Application::getInstance().getContextData());
    if(rcParams == nullptr) {
        throw std::runtime_error("No context specified");
    }
}

/**
 * @brief Destructor for a RestConfKeyController
 */
RestConfKeyController::~RestConfKeyController() { }

}
