/**
 * @file LoadYinController.cpp
 */

// Own includes
#include "controller/LoadYinController.h"
#include "gui/ListView.h"
#include "gui/ImageView.h"
#include "gui/TextView.h"
#include "Application.h"
#include "Utils.h"

// Defines
#define YIN_FOLDER         "yin/"
#define YIN_EXT            ".yin"
#define ICON_SIZE           81.0f
#define TEXT_OFFX           10.0f
#define TEXT_OFFY           5.0f
#define TEXT_SCALE          0.5f
#define MAX_PATH_LENGTH     27

namespace NetMan {

/**
 * @brief Go to the main menu
 */
static void goBack(void *args) {
    Application::getInstance().requestLayoutChange("menu");
}

/**
 * @brief Fill the modules list
 */
static void fillModules(void *args) {

    ListViewFillParams *params = (ListViewFillParams*)args;
    params->layouts.clear();
    auto controller = std::static_pointer_cast<LoadYinController>(params->controller);
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

/**
 * @brief Called when a module is clicked
 */
static void clickModule(void *args) {

    ListViewClickParams *params = (ListViewClickParams*)args;
    auto controller = std::static_pointer_cast<LoadYinController>(params->controller);

    std::shared_ptr<std::string> contextData = std::make_shared<std::string>(YIN_FOLDER + controller->getDirEntries()[params->element]);
    Application::getInstance().requestLayoutChange("yinbrowser", contextData);
}

/**
 * @brief Constructor for a LoadYinController
 */
LoadYinController::LoadYinController() {
    this->cbMap = std::unordered_map<std::string, void(*)(void*)> {
        {"goBack", goBack},
        {"fillModules", fillModules},
        {"clickModule", clickModule},
    };

    this->dirEntries = std::vector<std::string>();
    Utils::readFolder(YIN_FOLDER, YIN_EXT, this->dirEntries);
}

/**
 * @brief Destructor for a LoadYinController
 */
LoadYinController::~LoadYinController() { }

}
