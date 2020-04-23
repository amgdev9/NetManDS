/**
 * @file ListView.cpp
 * @brief View for lists
 */

// Includes
#include <stdexcept>
#include <math.h>

// Own includes
#include "gui/ListView.h"
#include "Application.h"

using namespace tinyxml2;

namespace NetMan {

/**
 * @brief Load a list view
 * @param node          XML node containing button parameters
 * @param controller    Controller used (can be nullptr)
 */
ListView::ListView(XMLElement *node, std::shared_ptr<GuiController> controller) {

    // Get attributes
    fillParams.startX = node->FloatAttribute("x", 0.0f);
    fillParams.startY = node->FloatAttribute("y", 0.0f);
    fillParams.layouts = std::vector<std::shared_ptr<GuiLayout>>();
    fillParams.remaining = true;
    fillParams.maxElements = node->IntAttribute("maxElements", 1);
    fillParams.elementWidth = node->FloatAttribute("width", 32.0f);
    fillParams.elementHeight = node->FloatAttribute("height", 32.0f);

    // Get callbacks
    const char *onClickStr = node->Attribute("onClick");
    if(onClickStr) onClick = std::string(onClickStr);
    const char *onFillStr = node->Attribute("onFill");
    if(onFillStr) onFill = std::string(onFillStr);

    // Load image data
    try {
        arrowData = Application::getInstance().getImageResource("upArrow");
    } catch (const std::runtime_error &e) {
        throw;
    }

    // Set up image parameters
    C2D_SpriteFromSheet(&arrowParams, arrowData, 0);
    C2D_SpriteSetCenter(&arrowParams, 0.5f, 0.5f);
    C2D_SpriteSetPos(&arrowParams, node->FloatAttribute("arrowX", 0.0f), node->FloatAttribute("arrowY", 0.0f));
    C2D_SpriteSetRotation(&arrowParams, 0.0f);

    // Save controller
    this->controller = controller;

    // Call the filler
    fillParams.startElement = 0;
    fillParams.controller = controller;
    fillParams.endElement = fillParams.maxElements;
    if(controller != nullptr && onFillStr) {
        controller->callMethod(onFill, &fillParams);
    }
}

/**
 * @brief Process input
 */
void ListView::input(u32 held, u32 down, u32 up, touchPosition &touch) {
    
    if(controller == nullptr) return;

    auto &pos = arrowParams.params.pos;

    float hw = fabs(pos.w) / 2.0f;
    float hh = fabs(pos.h) / 2.0f;

    if((down &KEY_TOUCH) && touch.px >= (pos.x - hw) && touch.px <= (pos.x + hw) && !onFill.empty()) {
        if(touch.py >= (pos.y - hh) && touch.py <= (pos.y + hh) && (fillParams.startElement > 0)) {
            fillParams.startElement -= fillParams.maxElements;
            fillParams.endElement -= fillParams.maxElements;
            fillParams.remaining = true;
            fillParams.controller = controller;
            controller->callMethod(onFill, &fillParams);
        } else if(touch.py >= (pos.y + hh) && touch.py <= (pos.y + 3.0f * hh) && fillParams.remaining) {
            fillParams.startElement += fillParams.maxElements;
            fillParams.endElement += fillParams.maxElements;
            fillParams.remaining = true;
            fillParams.controller = controller;
            controller->callMethod(onFill, &fillParams);
        }
    } else if(!onClick.empty()) {
        for(u32 i = 0; i < fillParams.layouts.size(); i++) {
            float y = fillParams.startY + i * fillParams.elementHeight;
            if((down &KEY_TOUCH) && touch.px >= fillParams.startX && touch.px <= (fillParams.startX + fillParams.elementWidth) &&
               touch.py >= y && touch.py <= (y + fillParams.elementHeight)) {
                   ListViewClickParams params;
                   params.controller = controller;
                   params.element = fillParams.startElement + i;
                   params.startElement = fillParams.startElement;
                   params.changedLayout = false;
                   controller->callMethod(onClick, &params);
                   if(params.changedLayout) break;
            }
        }
    }
}

/**
 * @brief Draw a list view
 */
void ListView::draw() {

    float tmpy = arrowParams.params.pos.y;
    C2D_SpriteSetScale(&arrowParams, 1.0f, 1.0f);
    if(fillParams.startElement > 0) C2D_DrawSprite(&arrowParams);

    arrowParams.params.pos.y += arrowParams.params.pos.h;
    C2D_SpriteSetScale(&arrowParams, 1.0f, -1.0f);
    if(fillParams.remaining) C2D_DrawSprite(&arrowParams);
    arrowParams.params.pos.y = tmpy;

    for(auto& layout : fillParams.layouts) {
        layout->draw();
    }
}

/**
 * @brief Destructor for a list view
 */
ListView::~ListView() { }

}
