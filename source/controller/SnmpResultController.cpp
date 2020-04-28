/**
 * @file SnmpResultController.cpp
 */

// Own includes
#include "controller/SnmpResultController.h"
#include "gui/ListView.h"
#include "gui/TextView.h"
#include "gui/ImageView.h"
#include "Application.h"

// Defines
#define ICON_SIZE           81.0f
#define TEXT_OFFX           10.0f
#define TEXT_OFFY           5.0f
#define TEXT_SCALE          0.5f
#define MAX_STR_LENGTH      32

namespace NetMan {

/**
 * @brief Go to the SNMP menu, clearing PDU contents
 */
static void goBack(void *args) {
    Application::getInstance().getPduFields().clear();
    Application::getInstance().requestLayoutChange("snmp");
}

/**
 * @brief Fill the SNMP response result list
 */
static void fillResults(void *args) {
    ListViewFillParams *params = (ListViewFillParams*)args;
    auto controller = std::static_pointer_cast<SnmpResultController>(params->controller);
    params->layouts.clear();

    auto& pduFields = Application::getInstance().getPduFields();

    if(params->endElement >= pduFields.size()) {
        params->remaining = false;
    }

    for(u32 i = params->startElement; i < params->endElement; i++) {
        if(i < pduFields.size()) {
            float y = params->startY + (i % params->maxElements) * params->elementHeight;
            
            std::string valueStr = pduFields[i].oidText + ": " + pduFields[i].value;
            if(valueStr.length() > MAX_STR_LENGTH) {
                valueStr.resize(MAX_STR_LENGTH);
            }

            std::shared_ptr<GuiLayout> layout = std::make_shared<GuiLayout>();
            std::shared_ptr<ImageView> bg = std::make_shared<ImageView>("menuButton", params->startX + params->elementWidth / 2, y + params->elementHeight / 2, params->elementWidth / ICON_SIZE, params->elementHeight / ICON_SIZE);
            std::shared_ptr<TextView> tv = std::make_shared<TextView>(valueStr, params->startX + TEXT_OFFX, y + TEXT_OFFY, TEXT_SCALE, C2D_Color32(0, 0, 0, 0xFF));
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
 * @brief Called when a list entry is clicked
 */
static void clickResult(void *args) {
    ListViewClickParams *params = (ListViewClickParams*)args;
    auto controller = std::static_pointer_cast<SnmpResultController>(params->controller);
    auto& pduFields = Application::getInstance().getPduFields();
    Application::getInstance().messageBox(pduFields[params->element].oidText + ": " + pduFields[params->element].value);
}

/**
 * @brief Constructor for a SnmpResultController
 */
SnmpResultController::SnmpResultController() {
    this->cbMap = std::unordered_map<std::string, void(*)(void*)> {
        {"goBack", goBack},
        {"fillResults", fillResults},
        {"clickResult", clickResult},
    };
}

/**
 * @brief Destructor for a SnmpResultController
 */
SnmpResultController::~SnmpResultController() { }

}
