/**
 * @file RestConfOpController.cpp
 */

// Own includes
#include "controller/RestConfOpController.h"
#include "gui/ButtonView.h"
#include "Application.h"
#include "restconf/RestConfClient.h"
#include "Config.h"

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

    if(controller->getRestConfParams()->isList) {
        // TODO List
    } else {
        auto& config = Config::getInstance();
        try {
            auto restConfClient = std::make_shared<RestConfClient>(config.getRestConfURL(), config.getRestConfUser(), config.getRestConfPassword(), config.getData().rcTimeout);
            auto jsonData = restConfClient->request(controller->getRestConfParams()->url, HTTPC_METHOD_GET);
            char *response = json_dumps(jsonData.get(), JSON_INDENT(2));
            Application::getInstance().messageBox(std::string("Received response:\n") + response);
            free(response);
        } catch (const std::runtime_error &e) {
            Application::getInstance().messageBox(e.what());
        }
    }
}

/**
 * @brief Perform a RESTCONF POST request
 */
static void postRequest(void *args) {
    // TODO Post
}

/**
 * @brief Perform a RESTCONF DELETE request
 */
static void deleteRequest(void *args) {
    ButtonParams *params = (ButtonParams*)args;
    auto controller = std::static_pointer_cast<RestConfOpController>(params->controller);

    if(controller->getRestConfParams()->isList) {
        // TODO List
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
