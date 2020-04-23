/**
 * @file SnmpParamsController.cpp
 */

// Own includes
#include "controller/SnmpParamsController.h"

namespace NetMan {

static void goBack(void *args) {

}

SnmpParamsController::SnmpParamsController() {

    this->cbMap = std::unordered_map<std::string, void(*)(void*)> {
        {"goBack", goBack},
    };
}

SnmpParamsController::~SnmpParamsController() { }

}
