/**
 * @file BinaryButtonView.cpp
 * @brief View for binary buttons
 */

// Includes
#include <stdexcept>
#include <math.h>

// Own includes
#include "gui/BinaryButtonView.h"
#include "Application.h"

using namespace tinyxml2;

namespace NetMan {

/**
 * @brief Load a binary button view
 * @param node          XML node containing button parameters
 * @param controller    Controller used (can be nullptr)
 */
BinaryButtonView::BinaryButtonView(XMLElement *node, std::shared_ptr<GuiController> controller) : ImageView(node, controller) {
    
    C2D_PlainImageTint(&tints[0], C2D_Color32(0xFF, 0xFF, 0xFF, 0xFF), 0.0f);
    C2D_PlainImageTint(&tints[1], C2D_Color32(0x0F, 0x0F, 0x0F, 0xFF), 0.2f);

    selected = false;

    const char *onClickStr = node->Attribute("onClick");
    if(onClickStr) {
        onClick = std::string(onClickStr);
    }

    // Call the controller to initialize the field
    if(controller != nullptr && !onClick.empty()) {
        BinaryButtonParams params;
        params.controller = controller;
        params.selected = false;
        params.init = false;
        controller->callMethod(onClick, &params);
        if(params.init) {
            selected = params.selected;
        }
    }
}

/**
 * @brief Process input
 */
void BinaryButtonView::input(u32 held, u32 down, u32 up, touchPosition &touch) {
    
    auto &pos = imageParams.params.pos;

    float hw = fabs(pos.w) / 2.0f;
    float hh = fabs(pos.h) / 2.0f;

    if((down &KEY_TOUCH) && touch.py >= (pos.y - hh) && touch.py <= (pos.y + hh)) {
        if(touch.px >= (pos.x - hw) && touch.px <= (pos.x + hw) && selected) {
            selected = false;
            if(controller != nullptr && !onClick.empty()) {
                BinaryButtonParams params;
                params.controller = controller;
                params.selected = selected;
                params.init = true;
                controller->callMethod(onClick, &params);
            }
        } else if(touch.px >= (pos.x + hw) && touch.px <= (pos.x + 3.0f * hw) && !selected) {
            selected = true;
            if(controller != nullptr && !onClick.empty()) {
                BinaryButtonParams params;
                params.controller = controller;
                params.selected = selected;
                params.init = true;
                controller->callMethod(onClick, &params);
            }
        }
    }
}

/**
 * @brief Draw a button view
 */
void BinaryButtonView::draw() {
    float tmpx = imageParams.params.pos.x;
    C2D_DrawSpriteTinted(&imageParams, &tints[!selected]);

    imageParams.params.pos.x += imageParams.params.pos.w;
    C2D_DrawSpriteTinted(&imageParams, &tints[selected]);
    imageParams.params.pos.x = tmpx;
}

/**
 * @brief Destructor for a button view
 */
BinaryButtonView::~BinaryButtonView() { }

}
