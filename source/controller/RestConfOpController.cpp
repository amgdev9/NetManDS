/**
 * @file RestConfOpController.cpp
 */

// Own includes
#include "controller/RestConfOpController.h"
#include "Application.h"

namespace NetMan {

static void goBack(void *args) {
    Application::getInstance().requestLayoutChange("loadyin");
}

static void getRequest(void *args) {

}

static void postRequest(void *args) {

}

static void deleteRequest(void *args) {

}

RestConfOpController::RestConfOpController() {
    this->cbMap = std::unordered_map<std::string, void(*)(void*)> {
        {"goBack", goBack},
        {"getRequest", getRequest},
        {"postRequest", postRequest},
        {"deleteRequest", deleteRequest},
    };

    restConfParams = std::static_pointer_cast<RestConfParams>(Application::getInstance().getContextData());
    if(restConfParams == nullptr) {
        throw std::runtime_error("No context specified");
    }
}

RestConfOpController::~RestConfOpController() { }

}
