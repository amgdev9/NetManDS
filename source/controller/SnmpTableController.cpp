/**
 * @file SnmpTableController.cpp
 */

// Own includes
#include "controller/SnmpTableController.h"
#include "controller/SnmpTypeController.h"
#include "gui/ButtonView.h"
#include "Application.h"
#include "snmp/Snmpv1Pdu.h"
#include "Utils.h"

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

namespace NetMan {

// Static data
std::shared_ptr<SnmpThreadParams> SnmpTableController::snmpParams = nullptr;

/**
 * @brief Go to the SNMP menu, clearing PDU contents
 */
static void goBack(void *args) {
    ButtonParams *params = (ButtonParams*)args;
    auto controller = std::static_pointer_cast<SnmpTableController>(params->controller);
    controller->resetSnmpParams();
    Application::getInstance().getPduFields().clear();
    Application::getInstance().requestLayoutChange("snmp");
}

/**
 * @brief Fill the SNMP table contents for a specific row
 */
static void fillTable(void *args) {

    ListViewFillParams *params = (ListViewFillParams*)args;
    params->layouts.clear();
    auto controller = std::static_pointer_cast<SnmpTableController>(params->controller);
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
            std::string fieldText = pduFields[i].oidText + ": " + pduFields[i].value;
            fieldText.resize(MAX_TEXT_LENGTH);
            std::shared_ptr<ImageView> bg = std::make_shared<ImageView>("menuButton", params->startX + params->elementWidth / 2, y + params->elementHeight / 2, params->elementWidth / ICON_SIZE, params->elementHeight / ICON_SIZE);
            std::shared_ptr<TextView> oidText = std::make_shared<TextView>(fieldText, params->startX + TEXT_OFFX, y + TEXT_OFFY, TEXT_SCALE, C2D_Color32(0, 0, 0, 0xFF));
            std::shared_ptr<ImageView> typeBox = std::make_shared<ImageView>("menuButton", params->startX + params->elementWidth - TYPEBOX_XOFFS, y + params->elementHeight - TYPEBOX_YOFFS, 0.25f, 0.25f);
            std::shared_ptr<TextView> typeText = std::make_shared<TextView>(std::string(1, pduFields[i].type), params->startX + params->elementWidth - TYPETEXT_XOFFS, y + params->elementHeight - TYPETEXT_YOFFS, TEXT_SCALE, C2D_Color32(0, 0, 0, 0xFF));
            std::shared_ptr<EditTextView> valueEditText = std::make_shared<EditTextView>(params->startX + params->elementWidth - EDITTEXT_XOFFS, y - EDITTEXT_YOFFS, EDITTEXT_WIDTH, EDITTEXT_HEIGHT, EDITTEXT_LENGTH, controller, "onEditColumn");
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

/**
 * @brief Called when a table column is clicked
 */
static void clickColumn(void *args) {

    ListViewClickParams *params = (ListViewClickParams*)args;
    auto controller = std::static_pointer_cast<SnmpTableController>(params->controller);
    controller->setCurrentField(params->element);
    auto& icons = controller->getIcons();
    u32 down = Application::getInstance().getDown();
    u32 index = params->element - params->startElement;
    touchPosition &touch = Application::getInstance().getTouch();

    if(icons[index].valueEditText->touched(down, touch)) {
        icons[index].valueEditText->input(0, down, 0, touch);
    } else if(icons[index].typeBox->touched(down, touch)) {
        std::shared_ptr<SnmpTypeContext> contextData = std::make_shared<SnmpTypeContext>();
        contextData->element = params->element;
        contextData->comesFromTable = true;
        Application::getInstance().requestLayoutChange("snmptype", contextData);
    } else {
        auto& pduFields = Application::getInstance().getPduFields();
        Application::getInstance().messageBox(pduFields[params->element].oidText + ": " + pduFields[params->element].value);
    }
}

/**
 * @brief Called when a table column is edited
 */
static void onEditColumn(void *args) {
    
    EditTextParams *params = (EditTextParams*)args;
    if(!params->init) return;

    // Backup the existing PDU
    auto controller = std::static_pointer_cast<SnmpTableController>(params->controller);
    auto pduFieldsCopy = Application::getInstance().getPduFields();
    auto& pduFields = Application::getInstance().getPduFields();
    pduFields.clear();

    // Add an unique field
    pduFields.push_back(pduFieldsCopy[controller->getCurrentField()]);
    pduFields[0].value = std::string(params->text);

    // Send the SET request
    auto snmpParams = controller->getSnmpParams();
    snmpParams->session->pduType = SNMPV1_SETREQUEST;
    try {
        Utils::sendSnmpPdu(snmpParams.get());
    } catch (const std::runtime_error &e) {
        Application::getInstance().messageBox(e.what());
        pduFields = pduFieldsCopy;
        return;
    }

    // Restore the previous state
    pduFields = pduFieldsCopy;
    fillTable(controller->getFillParams());
}

/**
 * @brief Edit the table index
 */
static void editTableIndex(void *args) {
    EditTextParams *params = (EditTextParams*)args;
    auto controller = std::static_pointer_cast<SnmpTableController>(params->controller);
    Utils::handleFormInteger(params, controller->getTableIndex(), 999);
}

/**
 * @brief Obtain a row from the SNMP table
 */
static void getRow(void *args) {
    ButtonParams *params = (ButtonParams*)args;
    auto controller = std::static_pointer_cast<SnmpTableController>(params->controller);
    auto& pduFields = Application::getInstance().getPduFields();

    // Update the row to be used
    for(auto &field : pduFields) {
        field.oid->editLastElement(*controller->getTableIndex());
    }

    // Send the GET request
    auto snmpParams = controller->getSnmpParams();
    snmpParams->session->pduType = SNMPV1_GETREQUEST;
    try {
        Utils::sendSnmpPdu(snmpParams.get());
        fillTable(controller->getFillParams());
    } catch (const std::runtime_error &e) {
        Application::getInstance().messageBox(e.what());
    }
}

/**
 * @brief Constructor for a SnmpTableController
 */
SnmpTableController::SnmpTableController() {
    this->cbMap = std::unordered_map<std::string, void(*)(void*)> {
        {"goBack", goBack},
        {"fillTable", fillTable},
        {"clickColumn", clickColumn},
        {"editTableIndex", editTableIndex},
        {"getRow", getRow},
        {"onEditColumn", onEditColumn},
    };

    // Initialize the table, if needed
    if(snmpParams == nullptr) {

        snmpParams = std::static_pointer_cast<SnmpThreadParams>(Application::getInstance().getContextData());
        if(snmpParams == nullptr) {
            throw std::runtime_error("No context specified");
        }

        auto oidTree = snmpParams->session->mib->getOidTree();
        auto& pduFields = Application::getInstance().getPduFields();
        pduFields.clear();
        auto it = oidTree->find(snmpParams->session->tableName);
        auto columns = it->second->children.begin()->second->children;
        for(auto& column : columns) {
            PduField field;
            field.oid = snmpParams->session->mib->resolve(column.first);
            field.oid->addElement(0);   // Add index indicator
            field.oidText = column.first;
            field.type = '\0';
            field.value = "";
            pduFields.push_back(field);
        }
    }

    // Initialize data
    this->tableIndex = 1;
}

/**
 * @brief Add the icons for a table column
 * @param columnText    Descriptive column name
 * @param typeBox       Box which encapsulates column type
 * @param typeText      Column type
 * @param valueEditText Column value handler
 */
void SnmpTableController::addIcons(std::shared_ptr<TextView> columnText, std::shared_ptr<ImageView> typeBox, std::shared_ptr<TextView> typeText, std::shared_ptr<EditTextView> valueEditText) {
    SnmpTableIcons icons;
    icons.columnText = columnText;
    icons.typeBox = typeBox;
    icons.typeText = typeText;
    icons.valueEditText = valueEditText;
    this->tableIcons.push_back(icons);
}

/**
 * @brief Destructor for a SnmpTableController
 */
SnmpTableController::~SnmpTableController() { }

}
