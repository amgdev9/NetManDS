/**
 * @file MibBrowserController.cpp
 */

// Own includes
#include "controller/MibBrowserController.h"
#include "gui/ImageView.h"
#include "gui/TextView.h"
#include "gui/ButtonView.h"
#include "Application.h"
#include "Config.h"
#include "Utils.h"

// Defines
#define ICON_SIZE           81.0f
#define TEXT_OFFX           10.0f
#define TEXT_OFFY           5.0f
#define TEXT_SCALE          0.5f
#define MAX_TEXT_LENGTH     20
#define ICON_YOFFS          12.0f
#define ICON_XOFFS          16.0f
#define MAX_PDU_FIELDS      25

namespace NetMan {

/**
 * @brief Go to the SNMP menu
 */
static void goBack(void *args) {
    Application::getInstance().requestLayoutChange("snmp");
}

/**
 * @brief Fill the OID list
 */
static void fillEntries(void *args) {

    ListViewFillParams *params = (ListViewFillParams*)args;
    params->layouts.clear();
    auto controller = std::static_pointer_cast<MibBrowserController>(params->controller);
    controller->clearIcons();
    controller->setFillParams(params);
    auto tree = controller->getCurrentTree();
    if(tree == nullptr) {
        Application::getInstance().messageBox("Can't load MIB tree");
        goBack(NULL);
        return;
    }

    if(params->endElement >= tree->children.size()) {
        params->remaining = false;
    }

    auto treeit = tree->children.begin();
    for(u32 i = 0; i < params->startElement; i++, treeit++);

    for(u32 i = params->startElement; i < params->endElement; i++) {
        if(i < tree->children.size()) {
            float y = params->startY + (i % params->maxElements) * params->elementHeight;
            std::shared_ptr<ImageView> bg = std::make_shared<ImageView>("menuButton", params->startX + params->elementWidth / 2, y + params->elementHeight / 2, params->elementWidth / ICON_SIZE, params->elementHeight / ICON_SIZE);
            std::shared_ptr<TextView> tv = std::make_shared<TextView>(treeit->first, params->startX + TEXT_OFFX, y + TEXT_OFFY, TEXT_SCALE, C2D_Color32(0, 0, 0, 0xFF));
            std::shared_ptr<ImageView> add = std::make_shared<ImageView>("addicon", params->startX + params->elementWidth - ICON_XOFFS, y + params->elementHeight - ICON_YOFFS, 0.5f, 0.5f);
            std::shared_ptr<ImageView> tick = std::make_shared<ImageView>("tick", params->startX + params->elementWidth - 2 * ICON_XOFFS, y + params->elementHeight - ICON_YOFFS, 0.25f, 0.25f);
            std::shared_ptr<ImageView> table = nullptr;
            std::shared_ptr<GuiLayout> layout = std::make_shared<GuiLayout>();
            layout->addView(bg);
            layout->addView(tv);
            layout->addView(add);
            layout->addView(tick);
            if(Utils::endsWith(treeit->first, "Table") && treeit->second->children.size() == 1) {
                table = std::make_shared<ImageView>("tableicon", params->startX + params->elementWidth - 3 * ICON_XOFFS, y + params->elementHeight - ICON_YOFFS, 0.5f, 0.5f);
                layout->addView(table);
            }
            controller->addEntryIcons(tv, tick, add, table);
            params->layouts.push_back(layout);
            treeit ++;
        } else {
            i = params->endElement;
            params->remaining = false;
        }
    }
}

/**
 * @brief Refresh the OID list
 * @param controller    This controller
 * @param oid           MIB node to use from now
 */
static void refreshLayout(std::shared_ptr<MibBrowserController> controller, std::shared_ptr<MibOid> oid) {
    controller->setCurrentTree(oid);
    ListViewFillParams *listParams = controller->getFillParams();
    listParams->endElement -= listParams->startElement;
    listParams->startElement = 0;
    listParams->remaining = true;
    fillEntries(listParams);
}

/**
 * @brief Called when a MIB node is clicked
 */
static void clickEntry(void *args) {

    ListViewClickParams *params = (ListViewClickParams*)args;
    auto controller = std::static_pointer_cast<MibBrowserController>(params->controller);
    auto& icons = controller->getEntryIcons();
    u32 down = Application::getInstance().getDown();
    u32 index = params->element - params->startElement;
    touchPosition &touch = Application::getInstance().getTouch();

    if(icons[index].add->touched(down, touch)) {
        if(Application::getInstance().getPduFields().size() < MAX_PDU_FIELDS) {
            PduField pduField;
            pduField.oid = controller->getMib()->resolve(icons[index].text->getText());
            pduField.oidText = icons[index].text->getText();
            pduField.type = 0;
            pduField.value = "";
            Application::getInstance().getPduFields().push_back(pduField);
            Application::getInstance().messageBox("Field added successfully");
        } else {
            Application::getInstance().messageBox("Limit of " + std::to_string(MAX_PDU_FIELDS) + " fields exceeded");
        }
    } else if(icons[index].table != nullptr && icons[index].table->touched(down, touch)) {
        std::shared_ptr<SnmpSessionParams> contextData = std::make_shared<SnmpSessionParams>();
        contextData->isTable = true;
        contextData->mib = controller->getMib();
        contextData->tableName = icons[index].text->getText();
        Application::getInstance().requestLayoutChange("agentview", contextData);
    } else if(icons[index].tick->touched(down, touch)) {
        auto mib = controller->getMib();
        auto it = mib->getOidTree()->find(icons[index].text->getText());
        Application::getInstance().messageBox(mib->getDescription(it->second));
    } else {
        auto oidtree = controller->getMib()->getOidTree();
        auto it = oidtree->find(icons[index].text->getText());
        if(it != oidtree->end()) {
            if(it->second->children.size() > 0) {
                refreshLayout(controller, it->second);
                params->changedLayout = true;
            }
        }
    }
}

/**
 * @brief Navigate to the previous MIB node
 */
static void prevEntry(void *args) {

    ButtonParams *params = (ButtonParams*)args;
    auto controller = std::static_pointer_cast<MibBrowserController>(params->controller);
    auto tree = controller->getCurrentTree();
    if(tree == nullptr) {
        goBack(NULL);
    }

    if(!tree->parentName.empty()) {
        auto oidtree = controller->getMib()->getOidTree();
        auto it = oidtree->find(tree->parentName);
        if(it != oidtree->end()) {
            refreshLayout(controller, it->second);
            return;
        }
    }

    Application::getInstance().messageBox("This node has no parent");
}

/**
 * @brief Constructor for a MibBrowserController
 */
MibBrowserController::MibBrowserController() {

    this->cbMap = std::unordered_map<std::string, void(*)(void*)> {
        {"fillEntries", fillEntries},
        {"clickEntry", clickEntry},
        {"goBack", goBack},
        {"prevEntry", prevEntry},
    };

    auto mibPath = std::static_pointer_cast<std::string>(Application::getInstance().getContextData());

    // Load the MIB file
    try {
        MibLoader &mibLoader = MibLoader::getInstance();
        mibLoader.loadSMI(Config::getInstance().getSmiPath());
        mib = mibLoader.load(*mibPath.get());
        auto oidTree = mib->getOidTree();
        auto it = oidTree->find("org");
        if(it != oidTree->end()) {
            currentTree = it->second;
        } else {
            currentTree = nullptr;
        }
    } catch (const std::runtime_error &e) {
        currentTree = nullptr;
    }
}

/**
 * @brief Add icons for a MIB entry
 * @param text  Descriptive OID name
 * @param tick  Tick icon used to show OID information
 * @param add   Add icon used to add the OID to a SNMP PDU
 * @param table Table icon used to view the OID as a SNMP table
 */
void MibBrowserController::addEntryIcons(std::shared_ptr<TextView> text, std::shared_ptr<ImageView> tick, std::shared_ptr<ImageView> add, std::shared_ptr<ImageView> table) {
    MibEntryIcons icons;
    icons.text = text;
    icons.tick = tick;
    icons.add = add;
    icons.table = table;
    this->entryIcons.push_back(icons);
}

/**
 * @brief Destructor for a MibBrowserController
 */
MibBrowserController::~MibBrowserController() { }

}
