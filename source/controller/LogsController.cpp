/**
 * @file LogsController.cpp
 */

// Own includes
#include "controller/LogsController.h"
#include "Application.h"

namespace NetMan {

static void gotoMenu(void *args) {
    Application::getInstance().requestLayoutChange("menu");
}

static void editLogMode(void *args) {

}

static void fillLogs(void *args) {

}

static void clickLog(void *args) {

}

LogsController::LogsController() {
    this->cbMap = std::unordered_map<std::string, void(*)(void*)> {
        {"gotoMenu", gotoMenu},
        {"editLogMode", editLogMode},
        {"fillLogs", fillLogs},
        {"clickLog", clickLog},
    };
}

LogsController::~LogsController() { }

}
