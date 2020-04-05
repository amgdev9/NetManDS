/**
 * @file MainController.cpp
 */

// Own includes
#include "controller/MainController.h"
#include "Application.h"

namespace NetMan {

MainController::MainController() {

}

MainController::~MainController() {

}

void MainController::initialize(std::vector<std::shared_ptr<GuiView>> &views) {

}

void MainController::callMethod(const std::string &method, void *args) {
    Application::getInstance().requestLayoutChange("menu");
}

}
