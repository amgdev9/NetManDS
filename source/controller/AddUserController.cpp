/**
 * @file AddUserController.cpp
 */

// Own includes
#include "controller/AddUserController.h"
#include "Application.h"
#include "gui/EditTextView.h"
#include "gui/ButtonView.h"

// Defines
#define SCREEN_SNMPV3_USERS     2

namespace NetMan {

/**
 * @brief Goto the options menu
 */
static void gotoOptions(void *args) {
    std::shared_ptr<u32> contextData = std::make_shared<u32>(SCREEN_SNMPV3_USERS);
    Application::getInstance().requestLayoutChange("options", contextData);
}

/**
 * @brief Edit the username field
 */
static void editUsername(void *args) {
    EditTextParams *params = (EditTextParams*)args;
    auto controller = std::static_pointer_cast<AddUserController>(params->controller);
    if(!params->init && Application::getInstance().getContextData() != nullptr) {
        sprintf(params->text, controller->getUsername().c_str());
        params->init = true;
    } else if(params->init) {
        controller->setUsername(std::string(params->text));
    }
}

/**
 * @brief Edit privacy protocol field
 */
static void editPriv(void *args) {
    EditTextParams *params = (EditTextParams*)args;
    auto controller = std::static_pointer_cast<AddUserController>(params->controller);
    Snmpv3UserStore &store = Snmpv3UserStore::getInstance();

    if(!params->init && Application::getInstance().getContextData() != nullptr) {
        sprintf(params->text, store.getPrivProtoString(controller->getUserEntry().privProto).c_str());
        params->init = true;
    } else if(params->init) {
        try {
            controller->setPrivProto(store.getPrivProtoID(std::string(params->text)));
        } catch (const std::runtime_error &e) {
            Application::getInstance().messageBox(e.what());
            params->init = false;
        }
    }
}

/**
 * @brief Edit privacy password field
 */
static void editPrivPass(void *args) {
    EditTextParams *params = (EditTextParams*)args;
    auto controller = std::static_pointer_cast<AddUserController>(params->controller);

    if(!params->init && Application::getInstance().getContextData() != nullptr) {
        sprintf(params->text, controller->getUserEntry().privPass.c_str());
        params->init = true;
    } else if(params->init) {
        controller->setPrivPass(std::string(params->text));
    }
}

/**
 * @brief Edit authentication protocol field
 */
static void editAuth(void *args) {
    EditTextParams *params = (EditTextParams*)args;
    auto controller = std::static_pointer_cast<AddUserController>(params->controller);
    Snmpv3UserStore &store = Snmpv3UserStore::getInstance();

    if(!params->init && Application::getInstance().getContextData() != nullptr) {
        sprintf(params->text, store.getAuthProtoString(controller->getUserEntry().authProto).c_str());
        params->init = true;
    } else if(params->init) {
        try {
            controller->setAuthProto(store.getAuthProtoID(std::string(params->text)));
        } catch (const std::runtime_error &e) {
            Application::getInstance().messageBox(e.what());
            params->init = false;
        }
    }
}

/**
 * @brief Edit authentication password field
 */
static void editAuthPass(void *args) {
    EditTextParams *params = (EditTextParams*)args;
    auto controller = std::static_pointer_cast<AddUserController>(params->controller);
    
    if(!params->init && Application::getInstance().getContextData() != nullptr) {
        sprintf(params->text, controller->getUserEntry().authPass.c_str());
        params->init = true;
    } else if(params->init) {
        controller->setAuthPass(std::string(params->text));
    }
}

/**
 * @brief Add an user to the store
 */
static void addUser(void *args) {
    ButtonParams *params = (ButtonParams*)args;
    auto controller = std::static_pointer_cast<AddUserController>(params->controller);
    Snmpv3UserStore &store = Snmpv3UserStore::getInstance();

    try {
        store.addUser(controller->getUsername(), controller->getUserEntry());
        store.save();
    } catch (const std::runtime_error &e) {
        Application::getInstance().messageBox(e.what());
    }

    std::shared_ptr<u32> contextData = std::make_shared<u32>(SCREEN_SNMPV3_USERS);
    Application::getInstance().requestLayoutChange("options", contextData);
}

/**
 * @brief Constructor for an AddUserController
 */
AddUserController::AddUserController() {
    this->cbMap = std::unordered_map<std::string, void(*)(void*)> {
        {"gotoOptions", gotoOptions},
        {"editUsername", editUsername},
        {"editPriv", editPriv},
        {"editPrivPass", editPrivPass},
        {"editAuth", editAuth},
        {"editAuthPass", editAuthPass},
        {"addUser", addUser},
    };

    if(Application::getInstance().getContextData() != nullptr) {
        std::shared_ptr<std::string> contextData = std::static_pointer_cast<std::string>(Application::getInstance().getContextData());
        auto user = Snmpv3UserStore::getInstance().getUser(*contextData.get());
        username = *contextData.get();
        userEntry.privProto = user.privProto;
        userEntry.privPass = user.privPass;
        userEntry.authProto = user.authProto;
        userEntry.authPass = user.authPass;
    } else {
        username = "";
        userEntry.privProto = SNMPV3_PRIVPROTO_NONE;
        userEntry.privPass = "";
        userEntry.authProto = SNMPV3_AUTHPROTO_NONE;
        userEntry.authPass = "";
    }
}

/**
 * @brief Destructor for an AddUserController
 */
AddUserController::~AddUserController() { }

}
