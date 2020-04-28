/**
 * @file ViewLogController.cpp
 */

// Own includes
#include "controller/ViewLogController.h"
#include "Application.h"
#include "gui/ListView.h"
#include "gui/ImageView.h"
#include "gui/TextView.h"

// Defines
#define ICON_SIZE           81.0f
#define TEXT_OFFX           10.0f
#define TEXT_SCALE          0.8f
#define MAX_STR_LENGTH      21

namespace NetMan {

/**
 * @brief Go to the logs screen
 */
static void gotoLogs(void *args) {
    Application::getInstance().requestLayoutChange("logs");
}

/**
 * @brief Fill the current log list
 */
static void fillLog(void *args) {

    ListViewFillParams *params = (ListViewFillParams*)args;
    auto controller = std::static_pointer_cast<ViewLogController>(params->controller);
    params->layouts.clear();

    auto list = controller->getJson();
    u32 listSize = json_array_size(list.get());

    if(params->endElement >= listSize) {
        params->remaining = false;
    }

    for(u32 i = params->startElement; i < params->endElement; i++) {
        if(i < listSize) {
            float y = params->startY + (i % params->maxElements) * params->elementHeight;
            json_t *obj = json_array_get(list.get(), i);
            std::string textContent = std::string(json_string_value(obj));
            if(textContent.length() > MAX_STR_LENGTH) {
                textContent.resize(MAX_STR_LENGTH);
            }

            std::shared_ptr<GuiLayout> layout = std::make_shared<GuiLayout>();
            std::shared_ptr<ImageView> bg = std::make_shared<ImageView>("menuButton", params->startX + params->elementWidth / 2, y + params->elementHeight / 2, params->elementWidth / ICON_SIZE, params->elementHeight / ICON_SIZE);
            std::shared_ptr<TextView> tv = std::make_shared<TextView>(textContent, params->startX + TEXT_OFFX, y, TEXT_SCALE, C2D_Color32(0, 0, 0, 0xFF));
            layout->addView(bg);
            layout->addView(tv);
            params->layouts.push_back(layout);
        } else {
            i = params->endElement;
            params->remaining = false;
        }
    }
}

/**
 * @brief Shows more detail about a log element
 */
static void clickElement(void *args) {
    ListViewClickParams *params = (ListViewClickParams*)args;
    auto controller = std::static_pointer_cast<ViewLogController>(params->controller);

    auto list = controller->getJson();
    json_t *obj = json_array_get(list.get(), params->element);

    if(obj) {
        Application::getInstance().messageBox(json_string_value(obj));
    }
}

/**
 * @brief Constructor for a ViewLogController
 */
ViewLogController::ViewLogController() {
    this->cbMap = std::unordered_map<std::string, void(*)(void*)> {
        {"gotoLogs", gotoLogs},
        {"fillLog", fillLog},
        {"clickElement", clickElement},
    };

    // Get log's root node
    root = std::static_pointer_cast<json_t>(Application::getInstance().getContextData());
}

/**
 * @brief Destructor for a ViewLogController
 */
ViewLogController::~ViewLogController() { }

}
