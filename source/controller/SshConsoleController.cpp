/**
 * @file SshConsoleController.cpp
 */

// Own includes
#include "controller/SshConsoleController.h"
#include "gui/ConsoleView.h"
#include "gui/EditTextView.h"
#include "gui/ButtonView.h"
#include "Application.h"

namespace NetMan {

// Static data to be shared between controllers
static std::string text;
static bool update;

/**
 * @brief Go to the main menu
 */
static void goMenu(void *args) {
    Application::getInstance().requestLayoutChange("menu");
}

/**
 * @brief Enter a terminal command
 */
static void enterCommand(void *args) {
    EditTextParams *params = (EditTextParams*)args;
    if(!params->init) return;
    auto controller = std::static_pointer_cast<SshConsoleController>(params->controller);
    text = std::string(params->text);
    update = true;
    params->init = false;
}

/**
 * @brief Update the console grid
 */
static void updateConsole(void *args) {
    ConsoleUpdateParams *params = (ConsoleUpdateParams*)args;
    auto controller = std::static_pointer_cast<SshConsoleController>(params->controller);
    auto helper = controller->getHelper();
    if(update) {
        if(helper->sendData(text + "\n")) {     // Send SSH command
            Application::getInstance().requestLayoutChange("menu");
        }
        update = false;
    }
    if(helper->recvData()) {
        params->text = helper->getRecvData();   // Receive SSH response
        params->update = true;
    }
}

/**
 * @brief Constructor for a SshConsoleController
 */
SshConsoleController::SshConsoleController() {
    this->cbMap = std::unordered_map<std::string, void(*)(void*)> {
        {"goMenu", goMenu},
        {"enterCommand", enterCommand},
        {"updateConsole", updateConsole},
    };
    update = false;
    text = "";
    this->helper = std::static_pointer_cast<SshHelper>(Application::getInstance().getContextData());
}

/**
 * @brief Destructor for a SshConsoleController
 */
SshConsoleController::~SshConsoleController() {
    helper->disconnect();
}

}
