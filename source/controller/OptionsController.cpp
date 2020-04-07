/**
 * @file OptionsController.cpp
 */

// Own includes
#include "controller/OptionsController.h"
#include "Application.h"
#include "gui/ListView.h"
#include "gui/TextView.h"
#include "gui/ImageView.h"

// Defines
#define ICON_SIZE   81.0f
#define TEXT_OFFX   10.0f
#define TEXT_SCALE  0.8f
#define CROSS_OFF   12.0f

namespace NetMan {

static void gotoMenu(void *args) {
    Application::getInstance().requestLayoutChange("menu");
}

static void goAddUser(void *args) {
    Application::getInstance().requestLayoutChange("adduser");
}

static void fillUsers(void *args) {

    ListViewFillParams *params = (ListViewFillParams*)args;
    params->layouts.clear();

    if(params->endElement >= 40) {
        params->remaining = false;
    }

    for(u32 i = params->startElement; i < params->endElement; i++) {
        if(i < 40) {
            float y = params->startY + (i % params->maxElements) * params->elementHeight;
            std::shared_ptr<ImageView> bg = std::make_shared<ImageView>("menuButton", params->startX + params->elementWidth / 2, y + params->elementHeight / 2, params->elementWidth / ICON_SIZE, params->elementHeight / ICON_SIZE);
            std::shared_ptr<TextView> tv = std::make_shared<TextView>("Text" + std::to_string(i), params->startX + TEXT_OFFX, y, TEXT_SCALE, C2D_Color32(0, 0, 0, 0xFF));
            std::shared_ptr<ImageView> cross = std::make_shared<ImageView>("cross", params->startX + params->elementWidth - CROSS_OFF, y + params->elementHeight - CROSS_OFF, 0.5f, 0.5f);
            std::shared_ptr<GuiLayout> layout = std::make_shared<GuiLayout>();
            layout->addView(bg);
            layout->addView(tv);
            layout->addView(cross);
            params->layouts.push_back(layout);
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
