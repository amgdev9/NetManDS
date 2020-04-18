/**
 * @file ButtonView.cpp
 * @brief View for buttons
 */

// Includes
#include <stdexcept>
#include <math.h>

// Own includes
#include "gui/ButtonView.h"
#include "Application.h"

using namespace tinyxml2;

namespace NetMan {

/**
 * @brief Load a button view
 * @param node          XML node containing button parameters
 * @param controller    Controller used (can be nullptr)
 */
ButtonView::ButtonView(XMLElement *node, std::shared_ptr<GuiController> controller) : ImageView(node, controller) {
    C2D_PlainImageTint(&tint, C2D_Color32(0xFF, 0xFF, 0xFF, 0xFF), 0.0f);

    const char *onClickStr = node->Attribute("onClick");
    if(onClickStr) {
        onClick = std::string(onClickStr);
    }
}

/**
 * @brief Process input
 */
void ButtonView::input(u32 held, u32 down, u32 up, touchPosition &touch) {

    if(this->touched(down, touch)) {
        C2D_PlainImageTint(&tint, C2D_Color32(0x0F, 0x0F, 0x0F, 0xFF), 0.2f);
        if(controller != nullptr && !onClick.empty()) {
            ButtonParams params;
            params.controller = controller;
            controller->callMethod(onClick, &params);
        }
    }

    if(up &KEY_TOUCH) {
        C2D_PlainImageTint(&tint, C2D_Color32(0xFF, 0xFF, 0xFF, 0xFF), 0.0f);
    }
}

/**
 * @brief Draw a button view
 */
void ButtonView::draw() {
    C2D_DrawSpriteTinted(&imageParams, &tint);
}

/**
 * @brief Destructor for a button view
 */
ButtonView::~ButtonView() { }

}
