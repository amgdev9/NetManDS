/**
 * @file CheckboxView.cpp
 * @brief View for checkboxes
 */

// Includes
#include <stdexcept>
#include <math.h>

// Own includes
#include "gui/CheckboxView.h"
#include "Application.h"

using namespace tinyxml2;

namespace NetMan {

/**
 * @brief Constructor for a CheckboxView
 * @param name          Resource name
 * @param x             X position
 * @param y             Y position
 * @param sx            X scale
 * @param sy            Y scale
 * @param controller    Controller to be used
 * @param onClick       onClick callback
 */
CheckboxView::CheckboxView(const char *name, float x, float y, float sx, float sy, std::shared_ptr<GuiController> controller, const std::string &onClick) : ImageView(name, x, y, sx, sy) {

    this->controller = controller;
    this->onClick = onClick;
    this->setup();
}

/**
 * @brief Load a checkbox view
 * @param node          XML node containing button parameters
 * @param controller    Controller used (can be nullptr)
 */
CheckboxView::CheckboxView(XMLElement *node, std::shared_ptr<GuiController> controller) : ImageView(node, controller) {

    const char *onClickStr = node->Attribute("onClick");
    if(onClickStr) {
        onClick = std::string(onClickStr);
    }

    this->setup();
}

/**
 * @brief Setup the CheckboxView
 */
void CheckboxView::setup() {

    // Load image data
    try {
        tickData = Application::getInstance().getImageResource("tick");
    } catch (const std::runtime_error &e) {
        throw;
    }

    // Set up image parameters
    C2D_SpriteFromSheet(&tickParams, tickData, 0);
    C2D_SpriteSetCenter(&tickParams, 0.5f, 0.5f);
    C2D_SpriteSetPos(&tickParams, imageParams.params.pos.x, imageParams.params.pos.y);
    C2D_SpriteSetRotation(&tickParams, 0.0f);
    C2D_SpriteSetScale(&tickParams, imageParams.params.pos.w / tickParams.params.pos.w, imageParams.params.pos.h / tickParams.params.pos.h);
    checked = false;

    // Call controller to initialize the field
    if(controller != nullptr && !onClick.empty()) {
        CheckboxParams params;
        params.state = false;
        params.init = false;
        params.controller = controller;
        controller->callMethod(onClick, &params);
        if(params.init) {
            checked = params.state;
        }
    }
}

/**
 * @brief Process input
 */
void CheckboxView::input(u32 held, u32 down, u32 up, touchPosition &touch) {
    
    auto &pos = imageParams.params.pos;

    float hw = fabs(pos.w) / 2.0f;
    float hh = fabs(pos.h) / 2.0f;

    if(touch.px >= (pos.x - hw) && touch.px <= (pos.x + hw) &&
       touch.py >= (pos.y - hh) && touch.py <= (pos.y + hh) && (down &KEY_TOUCH)) {
        checked = !checked;
        if(controller != nullptr && !onClick.empty()) {
            CheckboxParams params;
            params.state = checked;
            params.init = true;
            params.controller = controller;
            controller->callMethod(onClick, &params);
        }
    }
}

/**
 * @brief Draw a button view
 */
void CheckboxView::draw() {
    C2D_DrawSprite(&imageParams);
    if(checked) C2D_DrawSprite(&tickParams);
}

/**
 * @brief Destructor for a button view
 */
CheckboxView::~CheckboxView() { }

}
