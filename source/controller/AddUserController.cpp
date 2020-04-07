/**
 * @file AddUserController.cpp
 */

// Own includes
#include "controller/AddUserController.h"
#include "Application.h"
#include "gui/EditTextView.h"
#include "gui/ButtonView.h"

namespace NetMan {

static void gotoOptions(void *args) {
    Application::getInstance().requestLayoutChange("options");
}

static void editUsername(void *args) {
    EditTextParams *params = (EditTextParams*)args;
    std::static_pointer_cast<AddUserController>(params->controller)->setUsername(std::string(params->text));
}

static void editPriv(void *args) {
    EditTextParams *params = (EditTextParams*)args;
    Snmpv3UserStore &store = Snmpv3UserStore::getInstance();

    try {
        std::static_pointer_cast<AddUserController>(params->controller)->setPrivProto(store.getPrivProtoID(std::string(params->text)));
    } catch (const std::runtime_error &e) {
        Application::getInstance().messageBox(e.what());
        params->success = false;
    }
}

static void editPrivPass(void *args) {
    EditTextParams *params = (EditTextParams*)args;
    std::static_pointer_cast<AddUserController>(params->controller)->setPrivPass(std::string(params->text));
}

static void editAuth(void *args) {
    EditTextParams *params = (EditTextParams*)args;
    Snmpv3UserStore &store = Snmpv3UserStore::getInstance();

    try {
        std::static_pointer_cast<AddUserController>(params->controller)->setAuthProto(store.getAuthProtoID(std::string(params->text)));
    } catch (const std::runtime_error &e) {
        Application::getInstance().messageBox(e.what());
        params->success = false;
    }
}

static void editAuthPass(void *args) {
    EditTextParams *params = (EditTextParams*)args;
    std::static_pointer_cast<AddUserController>(params->controller)->setAuthPass(std::string(params->text));
}

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
    Application::getInstance().requestLayoutChange("options");
}

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
    username = "";
    userEntry.privProto = SNMPV3_PRIVPROTO_NONE;
    userEntry.privPass = "";
    userEntry.authProto = SNMPV3_AUTHPROTO_NONE;
    userEntry.authPass = "";
}

AddUserController::~AddUserController() { }

}
