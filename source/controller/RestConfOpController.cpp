/**
 * @file RestConfOpController.cpp
 */

// Own includes
#include "controller/RestConfOpController.h"
#include "Application.h"

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

}

/**
 * @brief Perform a RESTCONF POST request
 */
static void postRequest(void *args) {

}

/**
 * @brief Perform a RESTCONF DELETE request
 */
static void deleteRequest(void *args) {

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
