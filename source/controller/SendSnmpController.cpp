/**
 * @file SendSnmpController.cpp
 */

// Own includes
#include "controller/SendSnmpController.h"
#include "Application.h"

// Defines
#define ICON_SIZE           81.0f
#define TEXT_OFFX           10.0f
#define TEXT_OFFY           5.0f
#define TEXT_SCALE          0.5f
#define MAX_TEXT_LENGTH     17
#define TYPEBOX_XOFFS       74.0f
#define TYPEBOX_YOFFS       12.0f
#define TYPETEXT_XOFFS      77.0f
#define TYPETEXT_YOFFS      20.0f
#define EDITTEXT_XOFFS      62.0f
#define EDITTEXT_YOFFS      -2.0f
#define EDITTEXT_WIDTH      50.0f
#define EDITTEXT_HEIGHT     20.0f
#define EDITTEXT_LENGTH     4

namespace NetMan {

static void goBack(void *args) {
    Application::getInstance().requestLayoutChange("snmp");
}

static void fillFields(void *args) {

    ListViewFillParams *params = (ListViewFillParams*)args;
    params->layouts.clear();
    auto controller = std::static_pointer_cast<SendSnmpController>(params->controller);
    controller->clearIcons();
    controller->setFillParams(params);

    auto& pduFields = Application::getInstance().getPduFields();

    if(params->endElement >= pduFields.size()) {
        params->remaining = false;
    }

    for(u32 i = params->startElement; i < params->endElement; i++) {
        if(i < pduFields.size()) {
            controller->setCurrentField(i);
            float y = params->startY + (i % params->maxElements) * params->elementHeight;
            std::string fieldText = pduFields[i].oidText;
            fieldText.resize(MAX_TEXT_LENGTH);
            std::shared_ptr<ImageView> bg = std::make_shared<ImageView>("menuButton", params->startX + params->elementWidth / 2, y + params->elementHeight / 2, params->elementWidth / ICON_SIZE, params->elementHeight / ICON_SIZE);
            std::shared_ptr<TextView> oidText = std::make_shared<TextView>(fieldText, params->startX + TEXT_OFFX, y + TEXT_OFFY, TEXT_SCALE, C2D_Color32(0, 0, 0, 0xFF));
            std::shared_ptr<ImageView> typeBox = std::make_shared<ImageView>("menuButton", params->startX + params->elementWidth - TYPEBOX_XOFFS, y + params->elementHeight - TYPEBOX_YOFFS, 0.25f, 0.25f);
            std::shared_ptr<TextView> typeText = std::make_shared<TextView>(std::string(1, pduFields[i].type), params->startX + params->elementWidth - TYPETEXT_XOFFS, y + params->elementHeight - TYPETEXT_YOFFS, TEXT_SCALE, C2D_Color32(0, 0, 0, 0xFF));
            std::shared_ptr<EditTextView> valueEditText = std::make_shared<EditTextView>(params->startX + params->elementWidth - EDITTEXT_XOFFS, y - EDITTEXT_YOFFS, EDITTEXT_WIDTH, EDITTEXT_HEIGHT, EDITTEXT_LENGTH, controller, "onEditValue");
            std::shared_ptr<GuiLayout> layout = std::make_shared<GuiLayout>();
            layout->addView(bg);
            layout->addView(oidText);
            layout->addView(typeBox);
            layout->addView(typeText);
            layout->addView(valueEditText);
            controller->addIcons(oidText, typeBox, typeText, valueEditText);
            params->layouts.push_back(layout);
        } else {
            i = params->endElement;
            params->remaining = false;
        }
    }
}

static void clickField(void *args) {

    ListViewClickParams *params = (ListViewClickParams*)args;
    auto controller = std::static_pointer_cast<SendSnmpController>(params->controller);
    controller->setCurrentField(params->element);
    auto& icons = controller->getIcons();
    u32 down = Application::getInstance().getDown();
    u32 index = params->element - params->startElement;
    touchPosition &touch = Application::getInstance().getTouch();

    icons[index].valueEditText->input(0, down, 0, touch);

    if(icons[index].typeBox->touched(down, touch)) {
        // TODO
    }
}

static void onEditValue(void *args) {

}

static void snmpGet(void *args) {

}

static void snmpSet(void *args) {

}

static void snmpGetNext(void *args) {

}

static void snmpGetBulk(void *args) {

}

SendSnmpController::SendSnmpController() {
    this->cbMap = std::unordered_map<std::string, void(*)(void*)> {
        {"goBack", goBack},
        {"fillFields", fillFields},
        {"clickField", clickField},
        {"snmpGet", snmpGet},
        {"snmpSet", snmpSet},
        {"snmpGetNext", snmpGetNext},
        {"snmpGetBulk", snmpGetBulk},
        {"onEditValue", onEditValue},
    };

    // TODO Remove
    PduField field;
    field.oid = std::make_shared<BerOid>("1.2.3.4");
    field.type = 's';
    field.value = "";
    field.oidText = "sysUpTimesysUpTime";
    Application::getInstance().getPduFields().push_back(field);
    Application::getInstance().getPduFields().push_back(field);
}

void SendSnmpController::addIcons(std::shared_ptr<TextView> oidText, std::shared_ptr<ImageView> typeBox, std::shared_ptr<TextView> typeText, std::shared_ptr<EditTextView> valueEditText) {
    SendSnmpIcons icons;
    icons.oidText = oidText;
    icons.typeBox = typeBox;
    icons.typeText = typeText;
    icons.valueEditText = valueEditText;
    this->sendSnmpIcons.push_back(icons);
}

SendSnmpController::~SendSnmpController() { }

}