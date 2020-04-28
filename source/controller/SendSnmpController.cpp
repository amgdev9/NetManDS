/**
 * @file SendSnmpController.cpp
 */

// Own includes
#include "controller/SendSnmpController.h"
#include "controller/SnmpTypeController.h"
#include "Application.h"
#include "snmp/Snmpv2Pdu.h"

// Defines
#define ICON_SIZE           81.0f
#define TEXT_OFFX           10.0f
#define TEXT_OFFY           5.0f
#define TEXT_SCALE          0.5f
#define MAX_TEXT_LENGTH     16
#define TYPEBOX_XOFFS       84.0f
#define TYPEBOX_YOFFS       12.0f
#define TYPETEXT_XOFFS      87.0f
#define TYPETEXT_YOFFS      20.0f
#define EDITTEXT_XOFFS      72.0f
#define EDITTEXT_YOFFS      -2.0f
#define EDITTEXT_WIDTH      50.0f
#define EDITTEXT_HEIGHT     20.0f
#define EDITTEXT_LENGTH     4
#define CROSS_OFF           12.0f

namespace NetMan {

/**
 * @brief Go to the SNMP menu
 */
static void goBack(void *args) {
    Application::getInstance().requestLayoutChange("snmp");
}

/**
 * @brief Fill the SNMP PDU field list
 */
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
            std::shared_ptr<ImageView> cross = std::make_shared<ImageView>("cross", params->startX + params->elementWidth - CROSS_OFF, y + params->elementHeight - CROSS_OFF, 0.5f, 0.5f);
            std::shared_ptr<GuiLayout> layout = std::make_shared<GuiLayout>();
            layout->addView(bg);
            layout->addView(oidText);
            layout->addView(typeBox);
            layout->addView(typeText);
            layout->addView(valueEditText);
            layout->addView(cross);
            controller->addIcons(oidText, typeBox, typeText, valueEditText, cross);
            params->layouts.push_back(layout);
        } else {
            i = params->endElement;
            params->remaining = false;
        }
    }
}

/**
 * @brief Called when a SNMP PDU field is clicked
 */
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
        std::shared_ptr<SnmpTypeContext> contextData = std::make_shared<SnmpTypeContext>();
        contextData->element = params->element;
        contextData->comesFromTable = false;
        Application::getInstance().requestLayoutChange("snmptype", contextData);
    } else if(icons[index].cross->touched(down, touch)) {
        auto& pduFields = Application::getInstance().getPduFields();
        pduFields.erase(pduFields.begin() + params->element);
        fillFields(controller->getFillParams());
    }
}

/**
 * @brief Edit a SNMP PDU field value (used for SetRequest operations)
 */
static void onEditValue(void *args) {
    EditTextParams *params = (EditTextParams*)args;
    auto controller = std::static_pointer_cast<SendSnmpController>(params->controller);
    std::string &valueStr = Application::getInstance().getPduFields()[controller->getCurrentField()].value;

    if(!params->init) {
        sprintf(params->text, valueStr.c_str());
        params->init = true;
    } else {
        valueStr.assign(params->text);
    }
}

/**
 * @brief Go to another screen
 * @param pduType   SNMP PDU type to be sent
 * @param view      Screen name ("agentview" by default)
 */
static void gotoView(u32 pduType, const std::string &view = "agentview") {
    std::shared_ptr<SnmpSessionParams> sessionParams = std::make_shared<SnmpSessionParams>();
    sessionParams->isTable = false;
    sessionParams->pduType = pduType;
    Application::getInstance().requestLayoutChange(view, sessionParams);
}

/**
 * @brief Called when the GET button is pressed
 */
static void snmpGet(void *args) {
    gotoView(SNMPV1_GETREQUEST);
}

/**
 * @brief Called when the SET button is pressed
 */
static void snmpSet(void *args) {
    gotoView(SNMPV1_SETREQUEST);
}

/**
 * @brief Called when the GETNEXT button is pressed
 */
static void snmpGetNext(void *args) {
    gotoView(SNMPV1_GETNEXTREQUEST);
}

/**
 * @brief Called when the GETBULK button is pressed
 */
static void snmpGetBulk(void *args) {
    gotoView(SNMPV2_GETBULKREQUEST, "getbulk");
}

/**
 * @brief Constructor for a SendSnmpController
 */
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
}

/**
 * @brief Add icons for a list field
 * @param oidText       Descriptive OID name
 * @param typeBox       Box which encapsultes OID type
 * @param typeText      OID type
 * @param valueEditText OID value handler
 * @param cross         Cross icon to delete the field from the PDU
 */
void SendSnmpController::addIcons(std::shared_ptr<TextView> oidText, std::shared_ptr<ImageView> typeBox, std::shared_ptr<TextView> typeText, std::shared_ptr<EditTextView> valueEditText, std::shared_ptr<ImageView> cross) {
    SendSnmpIcons icons;
    icons.oidText = oidText;
    icons.typeBox = typeBox;
    icons.typeText = typeText;
    icons.valueEditText = valueEditText;
    icons.cross = cross;
    this->sendSnmpIcons.push_back(icons);
}

/**
 * @brief Destructor for a SendSnmpController
 */
SendSnmpController::~SendSnmpController() { }

}
