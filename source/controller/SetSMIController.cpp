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
#include "Application.h"
#include "Config.h"

// Defines
#define MIBS_FOLDER         "mibs/"
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

    Config &config = Config::getInstance();
    config.getSmiPath().assign(MIBS_FOLDER + controller->getDirEntries()[params->element]);
    Application::getInstance().requestLayoutChange("options");
}

static void gotoOptions(void *args) {
    Application::getInstance().requestLayoutChange("options");
}

SetSMIController::SetSMIController() {

    this->cbMap = std::unordered_map<std::string, void(*)(void*)> {
        {"fillMibs", fillMibs},
        {"clickMib", clickMib},
        {"gotoOptions", gotoOptions},
    };

    DIR *dp = opendir(MIBS_FOLDER);
    if(dp == NULL) {
        throw std::runtime_error(std::string("Couldn't open ") + MIBS_FOLDER);
    }

    struct dirent *entry;
    struct stat statbuf;
    while((entry = readdir(dp)) != NULL) {

        lstat(entry->d_name, &statbuf);

        if(!S_ISDIR(statbuf.st_mode)) {
            this->dirEntries.push_back(entry->d_name);
        }
    }

    closedir(dp);
}

SetSMIController::~SetSMIController() { }

}
