/**
 * @file LogsController.cpp
 */

// Own includes
#include "controller/LogsController.h"
#include "Application.h"
#include "gui/BinaryButtonView.h"
#include "gui/ListView.h"
#include "gui/TextView.h"

// Defines
#define ICON_SIZE           81.0f
#define TEXT_OFFX           10.0f
#define TEXT_SCALE          0.8f

namespace NetMan {

static void gotoMenu(void *args) {
    Application::getInstance().requestLayoutChange("menu");
}

static void fillLogs(void *args) {

    ListViewFillParams *params = (ListViewFillParams*)args;
    auto controller = std::static_pointer_cast<LogsController>(params->controller);
    controller->setFillParams(params);
    params->layouts.clear();

    auto list = controller->getJson();
    u32 listSize = json_array_size(list.get());

    if(params->endElement >= listSize) {
        params->remaining = false;
    }

    for(u32 i = params->startElement; i < params->endElement; i++) {
        if(i < listSize) {
            float y = params->startY + (i % params->maxElements) * params->elementHeight;
            json_t *obj = json_array_get(list.get(), listSize - i - 1);
            json_t *name = json_object_get(obj, "name");
            std::shared_ptr<GuiLayout> layout = std::make_shared<GuiLayout>();
            std::shared_ptr<ImageView> bg = std::make_shared<ImageView>("menuButton", params->startX + params->elementWidth / 2, y + params->elementHeight / 2, params->elementWidth / ICON_SIZE, params->elementHeight / ICON_SIZE);
            layout->addView(bg);
            if(name) {
                std::shared_ptr<TextView> tv = std::make_shared<TextView>(json_string_value(name), params->startX + TEXT_OFFX, y, TEXT_SCALE, C2D_Color32(0, 0, 0, 0xFF));
                layout->addView(tv);
            }
            params->layouts.push_back(layout);
        } else {
            i = params->endElement;
            params->remaining = false;
        }
    }
}

static void editLogMode(void *args) {
    BinaryButtonParams *params = (BinaryButtonParams*)args;
    if(!params->init) return;
    auto controller = std::static_pointer_cast<LogsController>(params->controller);
    
    ListViewFillParams *listParams = controller->getFillParams();
    listParams->endElement -= listParams->startElement;
    listParams->startElement = 0;
    listParams->remaining = true;
    if(params->selected) {
        controller->loadJsonList("syslog.json");
    } else {
        controller->loadJsonList("snmpTrap.json");
    }
    fillLogs(listParams);
}

static void clickLog(void *args) {
    ListViewClickParams *params = (ListViewClickParams*)args;
    auto controller = std::static_pointer_cast<LogsController>(params->controller);
    
    auto list = controller->getJson();
    u32 listSize = json_array_size(list.get());
    json_t *obj = json_array_get(list.get(), listSize - params->element - 1);
    json_t *data = json_object_get(obj, "data");
    data = json_incref(data);

    auto context = std::shared_ptr<json_t>(data, [=](json_t* data) { json_decref(data); });
    Application::getInstance().requestLayoutChange("viewlog", context);
}

LogsController::LogsController() {
    this->cbMap = std::unordered_map<std::string, void(*)(void*)> {
        {"gotoMenu", gotoMenu},
        {"editLogMode", editLogMode},
        {"fillLogs", fillLogs},
        {"clickLog", clickLog},
    };

    loadJsonList("snmpTrap.json");
}

void LogsController::loadJsonList(const std::string &path) {
    root = std::shared_ptr<json_t>(json_load_file(path.c_str(), 0, NULL), [=](json_t* data) { json_decref(data); });
    if(root.get() == NULL) {
        Application::getInstance().messageBox("Can't load JSON list: " + path);
    }
}

LogsController::~LogsController() { }

}
