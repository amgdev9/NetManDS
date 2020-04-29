/**
 * @file RestConfListController.cpp
 */

// Own includes
#include "controller/RestConfListController.h"
#include "gui/ButtonView.h"
#include "gui/ListView.h"
#include "gui/TextView.h"
#include "Application.h"

// Defines
#define ICON_SIZE           81.0f
#define TEXT_OFFX           10.0f
#define TEXT_OFFY           5.0f
#define TEXT_SCALE          0.5f
#define MAX_TEXT_LENGTH     32
#define ICON_YOFFS          12.0f
#define ICON_XOFFS          16.0f
#define CHECKBOX_SCALE      0.25f

namespace NetMan {

/**
 * @brief Go to the RESTCONF operations screen
 */
static void goBack(void *args) {
    ButtonParams *params = (ButtonParams*)args;
    auto controller = std::static_pointer_cast<RestConfListController>(params->controller);
    auto rcParams = controller->getRestConfParams();
    Application::getInstance().requestLayoutChange("restconfop", rcParams);
}

/**
 * @brief Fill the list with list elements
 */
static void fillList(void *args) {
    ListViewFillParams *params = (ListViewFillParams*)args;
    params->layouts.clear();
    auto controller = std::static_pointer_cast<RestConfListController>(params->controller);
    controller->clearIcons();
    auto& listElements = controller->getListElements();

    if(params->endElement >= listElements.size()) {
        params->remaining = false;
    }

    for(u32 i = params->startElement; i < params->endElement; i++) {
        if(i < listElements.size()) {
            float y = params->startY + (i % params->maxElements) * params->elementHeight;

            std::string textStr = listElements[i];
            if(textStr.length() > MAX_TEXT_LENGTH) {
                textStr.resize(MAX_TEXT_LENGTH);
            }

            std::shared_ptr<ImageView> bg = std::make_shared<ImageView>("menuButton", params->startX + params->elementWidth / 2, y + params->elementHeight / 2, params->elementWidth / ICON_SIZE, params->elementHeight / ICON_SIZE);
            std::shared_ptr<TextView> tv = std::make_shared<TextView>(textStr, params->startX + TEXT_OFFX, y + TEXT_OFFY, TEXT_SCALE, C2D_Color32(0, 0, 0, 0xFF));
            std::shared_ptr<CheckboxView> checkbox = std::make_shared<CheckboxView>("menuButton", params->startX + params->elementWidth - ICON_XOFFS, y + params->elementHeight - ICON_YOFFS, CHECKBOX_SCALE, CHECKBOX_SCALE, controller, "onTickElement");
            std::shared_ptr<GuiLayout> layout = std::make_shared<GuiLayout>();
            layout->addView(bg);
            layout->addView(tv);
            layout->addView(checkbox);
            controller->addEntryIcons(checkbox);
            params->layouts.push_back(layout);
        } else {
            i = params->endElement;
            params->remaining = false;
        }
    }
}

/**
 * @brief Called when an element is clicked
 */
static void clickElement(void *args) {
    ListViewClickParams *params = (ListViewClickParams*)args;
    auto controller = std::static_pointer_cast<RestConfListController>(params->controller);
    auto& icons = controller->getEntryIcons();
    u32 down = Application::getInstance().getDown();
    u32 index = params->element - params->startElement;
    touchPosition &touch = Application::getInstance().getTouch();

    controller->setCurrentElement(params->element);
    icons[index]->input(0, down, 0, touch);
}

/**
 * @brief Called when an element is ticked
 */
static void onTickElement(void *args) {
    CheckboxParams *params = (CheckboxParams*)args;
    auto controller = std::static_pointer_cast<RestConfListController>(params->controller);
    if(!params->init) return;
    controller->getListState().get()[controller->getCurrentElement()] = params->state;
}

/**
 * @brief Go to the Key-content screen
 */
static void gotoKey(void *args) {
    ButtonParams *params = (ButtonParams*)args;
    auto controller = std::static_pointer_cast<RestConfListController>(params->controller);
    auto rcParams = controller->getRestConfParams();
    auto& listElements = controller->getListElements();
    auto listState = controller->getListState();
    for(u32 i = 0; i < listElements.size(); i++) {
        if(listState.get()[i]) {
            rcParams->fields.push_back(listElements[i]);
        }
    }
    Application::getInstance().requestLayoutChange("restconfkey", rcParams);
}

/**
 * @brief Constructor for a RestConfListController
 */
RestConfListController::RestConfListController() {
    this->cbMap = std::unordered_map<std::string, void(*)(void*)> {
        {"goBack", goBack},
        {"fillList", fillList},
        {"clickElement", clickElement},
        {"gotoKey", gotoKey},
        {"onTickElement", onTickElement},
    };

    // Retrieve context data
    rcParams = std::static_pointer_cast<RestConfParams>(Application::getInstance().getContextData());
    if(rcParams == nullptr) {
        throw std::runtime_error("No context specified");
    }

    // Get list elements
    rcParams->yinHelper->getListFields(rcParams->node, this->listElements);
    this->listState = std::shared_ptr<bool>(new bool[this->listElements.size()]);
    memset(this->listState.get(), 0, this->listElements.size() * sizeof(bool));
}

/**
 * @brief Destructor for a RestConfListController
 */
RestConfListController::~RestConfListController() { }

}
