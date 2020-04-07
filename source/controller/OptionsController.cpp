/**
 * @file OptionsController.cpp
 */

// Own includes
#include "controller/OptionsController.h"
#include "Application.h"
#include "gui/ListView.h"
#include "gui/TextView.h"

namespace NetMan {

static void gotoMenu(void *args) {
    Application::getInstance().requestLayoutChange("menu");
}

static void goAddUser(void *args) {
    Application::getInstance().requestLayoutChange("adduser");
}

static void fillUsers(void *args) {

    ListViewFillParams *params = (ListViewFillParams*)args;
    params->elementWidth = 30;
    params->elementHeight = 30;
    params->remaining = true;
    params->views.clear();

    for(u32 i = params->startElement; i < params->endElement; i++) {
        if(i < 42) {
            float y = params->startY + (i % 5) * params->elementHeight;
            std::shared_ptr<TextView> tv = std::make_shared<TextView>("Text" + std::to_string(i), params->startX, y, 1.0f, 0xFFFFFFFF);
            params->views.push_back(tv);
        } else {
            i = params->endElement;
            params->remaining = false;
        }
    }
}

static void clickUser(void *args) {

}

OptionsController::OptionsController() {
    this->cbMap = std::unordered_map<std::string, void(*)(void*)> {
        {"gotoMenu", gotoMenu},
        {"goAddUser", goAddUser},
        {"fillUsers", fillUsers},
        {"clickUser", clickUser},
    };
}

OptionsController::~OptionsController() { }

}
