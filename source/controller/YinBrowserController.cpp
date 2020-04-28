/**
 * @file YinBrowserController.cpp
 */

// Own includes
#include "controller/YinBrowserController.h"
#include "gui/TextView.h"
#include "gui/ButtonView.h"
#include "Application.h"
#include "controller/RestConfOpController.h"

// Defines
#define ICON_SIZE           81.0f
#define TEXT_OFFX           10.0f
#define TEXT_OFFY           5.0f
#define TEXT_SCALE          0.5f
#define MAX_TEXT_LENGTH     32
#define ICON_YOFFS          12.0f
#define ICON_XOFFS          16.0f

using namespace tinyxml2;

namespace NetMan {

/**
 * @brief Go back to the Load YIN screen
 */
static void goBack(void *args) {
    Application::getInstance().requestLayoutChange("loadyin");
}

/**
 * @brief Fill the YIN module list with node entries
 */
static void fillEntries(void *args) {

    ListViewFillParams *params = (ListViewFillParams*)args;
    params->layouts.clear();
    auto controller = std::static_pointer_cast<YinBrowserController>(params->controller);
    controller->clearIcons();
    controller->setFillParams(params);
    auto node = controller->getCurrentNode();
    if(node == NULL) {
        Application::getInstance().messageBox("Can't load YIN node");
        goBack(NULL);
        return;
    }

    u32 nchildren = controller->getNodeNChildren(node);

    if(params->endElement >= nchildren) {
        params->remaining = false;
    }

    auto nodeit = node->FirstChildElement();
    for(u32 i = 0; i < params->startElement; i++, nodeit = nodeit->NextSiblingElement());

    for(u32 i = params->startElement; i < params->endElement; i++) {
        if(i < nchildren) {
            float y = params->startY + (i % params->maxElements) * params->elementHeight;

            std::string textStr = nodeit->Name();
            const char *nameStr = nodeit->Attribute("name");
            if(nameStr) {
                textStr.append(std::string(": ") + nameStr);
            }
            if(textStr.length() > MAX_TEXT_LENGTH) {
                textStr.resize(MAX_TEXT_LENGTH);
            }

            std::shared_ptr<ImageView> bg = std::make_shared<ImageView>("menuButton", params->startX + params->elementWidth / 2, y + params->elementHeight / 2, params->elementWidth / ICON_SIZE, params->elementHeight / ICON_SIZE);
            std::shared_ptr<TextView> tv = std::make_shared<TextView>(textStr, params->startX + TEXT_OFFX, y + TEXT_OFFY, TEXT_SCALE, C2D_Color32(0, 0, 0, 0xFF));
            std::shared_ptr<ImageView> add = nullptr; 
            std::shared_ptr<GuiLayout> layout = std::make_shared<GuiLayout>();
            layout->addView(bg);
            layout->addView(tv);
            if(strcmp(nodeit->Name(), "container") == 0 || strcmp(nodeit->Name(), "leaf") == 0 || strcmp(nodeit->Name(), "list") == 0) {
                add = std::make_shared<ImageView>("addicon", params->startX + params->elementWidth - ICON_XOFFS, y + params->elementHeight - ICON_YOFFS, 0.5f, 0.5f);
                layout->addView(add);
            }
            controller->addEntryIcons(add);
            params->layouts.push_back(layout);
            nodeit = nodeit->NextSiblingElement();
        } else {
            i = params->endElement;
            params->remaining = false;
        }
    }
}

/**
 * @brief Refresh the YIN Browser layout
 * @param controller    This controller
 * @param node          Current browser node
 */
static void refreshLayout(std::shared_ptr<YinBrowserController> controller, XMLElement *node) {
    controller->setCurrentNode(node);
    ListViewFillParams *listParams = controller->getFillParams();
    listParams->endElement -= listParams->startElement;
    listParams->startElement = 0;
    listParams->remaining = true;
    fillEntries(listParams);
}

/**
 * @brief Called when a node is clicked
 */
static void clickEntry(void *args) {

    ListViewClickParams *params = (ListViewClickParams*)args;
    auto controller = std::static_pointer_cast<YinBrowserController>(params->controller);
    auto& icons = controller->getEntryIcons();
    u32 down = Application::getInstance().getDown();
    u32 index = params->element - params->startElement;
    touchPosition &touch = Application::getInstance().getTouch();

    auto node = controller->getCurrentNode();
    auto nodeit = node->FirstChildElement();
    for(u32 i = 0; i < params->element; i++, nodeit = nodeit->NextSiblingElement());

    if(icons[index] != nullptr && icons[index]->touched(down, touch)) {
        try {
            std::shared_ptr<RestConfParams> rcParams = std::make_shared<RestConfParams>();
            rcParams->isList = (strcmp(nodeit->Name(), "list") == 0);
            auto yinHelper = controller->getYinHelper();
            if(strcmp(nodeit->Name(), "container") == 0) {
                rcParams->url = yinHelper->getRestConfContainerURL(nodeit);
            } else {
                rcParams->url = yinHelper->getRestConfURL(nodeit);
            }
            Application::getInstance().requestLayoutChange("restconfop", rcParams);
        } catch (const std::runtime_error &e) {
            Application::getInstance().messageBox(e.what());
        }
    } else {
        if(nodeit->FirstChildElement()) {
            refreshLayout(controller, nodeit);
            params->changedLayout = true;
        } else if(nodeit->GetText()){
            Application::getInstance().messageBox(nodeit->GetText());
        }
    }
}

/**
 * @brief Go back to the previous node
 */
static void prevEntry(void *args) {

    ButtonParams *params = (ButtonParams*)args;
    auto controller = std::static_pointer_cast<YinBrowserController>(params->controller);
    auto node = controller->getCurrentNode();
    if(node == NULL) {
        goBack(NULL);
    }

    if(node->Parent() && node->Parent()->ToElement()) {
        refreshLayout(controller, node->Parent()->ToElement());
    } else {
        Application::getInstance().messageBox("This node has no parent");
    }
}

/**
 * @brief Constructor for a YinBrowserController
 */
YinBrowserController::YinBrowserController() {
    this->cbMap = std::unordered_map<std::string, void(*)(void*)> {
        {"fillEntries", fillEntries},
        {"clickEntry", clickEntry},
        {"goBack", goBack},
        {"prevEntry", prevEntry},
    };

    auto yinPath = std::static_pointer_cast<std::string>(Application::getInstance().getContextData());

    // Load the YIN module
    try {
        yinHelper = std::make_shared<YinHelper>(*yinPath.get());
        currentNode = yinHelper->getRoot();
    } catch (const std::runtime_error &e) {
        currentNode = nullptr;
    }
}

/**
 * @brief Get the number of children of a node
 * @param node  Node to be queried
 * @return The number of children of the node
 */
u32 YinBrowserController::getNodeNChildren(tinyxml2::XMLElement *node) {

    u32 nchildren = 0;
    for(auto child = node->FirstChildElement(); child; child = child->NextSiblingElement()) {
        nchildren ++;
    }

    return nchildren;
}

/**
 * @brief Destructor for a YinBrowserController
 */
YinBrowserController::~YinBrowserController() { }

}
