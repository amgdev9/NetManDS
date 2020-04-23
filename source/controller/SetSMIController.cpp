/**
 * @file SetSMIController.cpp
 */

// Includes C/C++
#include <dirent.h>

// Own includes
#include "controller/SetSMIController.h"
#include "gui/ListView.h"
#include "gui/ImageView.h"
#include "gui/TextView.h"
#include "gui/ButtonView.h"
#include "Application.h"
#include "Config.h"
#include "Utils.h"

// Defines
#define MIBS_FOLDER         "mibs/"
#define MIBS_EXT            ".txt"
#define ICON_SIZE           81.0f
#define TEXT_OFFX           10.0f
#define TEXT_OFFY           5.0f
#define TEXT_SCALE          0.5f
#define MAX_PATH_LENGTH     27

namespace NetMan {

static void fillMibs(void *args) {

    ListViewFillParams *params = (ListViewFillParams*)args;
    params->layouts.clear();
    auto controller = std::static_pointer_cast<SetSMIController>(params->controller);
    auto dirEntries = controller->getDirEntries();

    if(params->endElement >= dirEntries.size()) {
        params->remaining = false;
    }

    for(u32 i = params->startElement; i < params->endElement; i++) {
        if(i < dirEntries.size()) {
            float y = params->startY + (i % params->maxElements) * params->elementHeight;
            std::string path = dirEntries[i];
            path.resize(MAX_PATH_LENGTH);
            std::shared_ptr<ImageView> bg = std::make_shared<ImageView>("menuButton", params->startX + params->elementWidth / 2, y + params->elementHeight / 2, params->elementWidth / ICON_SIZE, params->elementHeight / ICON_SIZE);
            std::shared_ptr<TextView> tv = std::make_shared<TextView>(path, params->startX + TEXT_OFFX, y + TEXT_OFFY, TEXT_SCALE, C2D_Color32(0, 0, 0, 0xFF));
            std::shared_ptr<GuiLayout> layout = std::make_shared<GuiLayout>();
            layout->addView(bg);
            layout->addView(tv);
            params->layouts.push_back(layout);
        } else {
            i = params->endElement;
            params->remaining = false;
        }
    }
}

static void clickMib(void *args) {

    ListViewClickParams *params = (ListViewClickParams*)args;
    auto controller = std::static_pointer_cast<SetSMIController>(params->controller);

    if(controller->getComesFromOptions()) {
        Config &config = Config::getInstance();
        config.getSmiPath().assign(MIBS_FOLDER + controller->getDirEntries()[params->element]);
        Application::getInstance().requestLayoutChange("options");
    } else {
        std::shared_ptr<std::string> contextData = std::make_shared<std::string>(MIBS_FOLDER + controller->getDirEntries()[params->element]);
        Application::getInstance().requestLayoutChange("mibbrowser", contextData);
    }
}

static void goBack(void *args) {
    ButtonParams *params = (ButtonParams*)args;
    auto controller = std::static_pointer_cast<SetSMIController>(params->controller);
    if(controller->getComesFromOptions()) {
        Application::getInstance().requestLayoutChange("options");
    } else {
        Application::getInstance().requestLayoutChange("snmp");
    }
}

SetSMIController::SetSMIController() {

    this->cbMap = std::unordered_map<std::string, void(*)(void*)> {
        {"fillMibs", fillMibs},
        {"clickMib", clickMib},
        {"goBack", goBack},
    };

    this->dirEntries = std::vector<std::string>();
    Utils::readFolder(MIBS_FOLDER, MIBS_EXT, this->dirEntries);

    if(Application::getInstance().getContextData() == nullptr) {
        comesFromOptions = true;
    } else {
        comesFromOptions = false;
    }
}

SetSMIController::~SetSMIController() { }

}
