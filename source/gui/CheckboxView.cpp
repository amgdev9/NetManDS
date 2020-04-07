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
 * @brief Load a checkbox view
 * @param node          XML node containing button parameters
 * @param controller    Controller used (can be nullptr)
 */
CheckboxView::CheckboxView(XMLElement *node, std::shared_ptr<GuiController> controller) : ImageView(node, controller) {

    const char *onClickStr = node->Attribute("onClick");
    if(onClickStr) {
        onClick = std::string(onClickStr);
    }

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
            controller->callMethod(onClick, &checked);
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
