/**
 * @file SshController.cpp
 */

// Own includes
#include "controller/SshController.h"
#include "gui/EditTextView.h"
#include "gui/ButtonView.h"
#include "Application.h"

namespace NetMan {

static void goMenu(void *args) {
    Application::getInstance().requestLayoutChange("menu");
}

static void editHost(void *args) {
    EditTextParams *params = (EditTextParams*)args;
    if(!params->init) return;
    auto controller = std::static_pointer_cast<SshController>(params->controller);
    controller->getSession().host.assign(params->text);
}

static void editPort(void *args) {
    EditTextParams *params = (EditTextParams*)args;
    if(!params->init) return;
    auto controller = std::static_pointer_cast<SshController>(params->controller);
    int port = strtol(params->text, NULL, 10);
    if(port == 0 || port > 0xFFFF) {
        params->init = false;
        Application::getInstance().messageBox("Invalid port");
    } else {
        controller->getSession().port = (u16)port;
    }
}

static void editUsername(void *args) {
    EditTextParams *params = (EditTextParams*)args;
    if(!params->init) return;
    auto controller = std::static_pointer_cast<SshController>(params->controller);
    controller->getSession().username.assign(params->text);
}

static void editPassword(void *args) {
    EditTextParams *params = (EditTextParams*)args;
    if(!params->init) return;
    auto controller = std::static_pointer_cast<SshController>(params->controller);
    controller->getSession().password.assign(params->text);
}

static void connect(void *args) {
    ButtonParams *params = (ButtonParams*)args;
    auto controller = std::static_pointer_cast<SshController>(params->controller);
    std::shared_ptr<SshSession> contextData = std::make_shared<SshSession>(controller->getSession());
    Application::getInstance().requestLayoutChange("sshconsole", contextData);
}

SshController::SshController() {
    this->cbMap = std::unordered_map<std::string, void(*)(void*)> {
        {"goMenu", goMenu},
        {"editHost", editHost},
        {"editPort", editPort},
        {"editUsername", editUsername},
        {"editPassword", editPassword},
        {"connect", connect},
    };
}

SshController::~SshController() { }

}
