/**
 * @file SnmpTypeController.cpp
 */

// Own includes
#include "controller/SnmpTypeController.h"
#include "gui/ListView.h"
#include "gui/ImageView.h"
#include "gui/TextView.h"
#include "Application.h"

// Defines
#define ICON_SIZE           81.0f
#define TEXT_OFFX           10.0f
#define TEXT_OFFY           5.0f
#define TEXT_SCALE          0.5f

namespace NetMan {

static void goBack(void *args) {
    Application::getInstance().requestLayoutChange("sendsnmp");
}

static void fillTypes(void *args) {
    ListViewFillParams *params = (ListViewFillParams*)args;
    params->layouts.clear();
    
    static const char *snmpTypes[] = {
        "Integer",
        "Octet String",
        "Null",
        "Object Identifier",
        "Network Address",
        "Counter",
        "Counter64 (v2+)",
        "Gauge",
        "TimeTicks",
        "Opaque",
    };

    if(params->endElement >= sizeof(snmpTypes) / sizeof(char*)) {
        params->remaining = false;
    }

    for(u32 i = params->startElement; i < params->endElement; i++) {
        if(i < sizeof(snmpTypes) / sizeof(char*)) {
            float y = params->startY + (i % params->maxElements) * params->elementHeight;
            std::shared_ptr<ImageView> bg = std::make_shared<ImageView>("menuButton", params->startX + params->elementWidth / 2, y + params->elementHeight / 2, params->elementWidth / ICON_SIZE, params->elementHeight / ICON_SIZE);
            std::shared_ptr<TextView> tv = std::make_shared<TextView>(snmpTypes[i], params->startX + TEXT_OFFX, y + TEXT_OFFY, TEXT_SCALE, C2D_Color32(0, 0, 0, 0xFF));
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

static void clickType(void *args) {
    ListViewClickParams *params = (ListViewClickParams*)args;
    auto controller = std::static_pointer_cast<SnmpTypeController>(params->controller);

    static const char snmpTypeChars[] = {
        'i', 's', 'n', 'O', 'a', 'c', 'C', 'g', 't', 'o'
    };

    Application::getInstance().getPduFields()[controller->getField()].type = snmpTypeChars[params->element];
    Application::getInstance().requestLayoutChange("sendsnmp");
}

SnmpTypeController::SnmpTypeController() {
    this->cbMap = std::unordered_map<std::string, void(*)(void*)> {
        {"goBack", goBack},
        {"fillTypes", fillTypes},
        {"clickType", clickType},
    };

    auto contextData = Application::getInstance().getContextData();
    if(contextData == nullptr) {
        throw std::runtime_error("No context specified");
    }

    field = *std::static_pointer_cast<u32>(contextData).get();
}

SnmpTypeController::~SnmpTypeController() { }

}
