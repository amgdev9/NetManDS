/**
 * @file ConsoleView.cpp
 * @brief View for text consoles
 */

// Own includes
#include "gui/ConsoleView.h"
#include "Application.h"

using namespace tinyxml2;

namespace NetMan {

/**
 * @brief Load a console view
 * @param node          XML node containing button parameters
 * @param controller    Controller used (can be nullptr)
 */
ConsoleView::ConsoleView(XMLElement *node, std::shared_ptr<GuiController> controller) {

    // Load image data
    try {
        fontData = Application::getInstance().getImageResource("consoleFont");
    } catch (const std::runtime_error &e) {
        throw;
    }

    // Set up image parameters
    C2D_SpriteFromSheet(&fontParams, fontData, 0);
    C2D_SpriteSetCenter(&fontParams, 0.5f, 0.5f);
    C2D_SpriteSetPos(&fontParams, node->FloatAttribute("x", 0.0f), node->FloatAttribute("y", 0.0f));
    C2D_SpriteSetRotation(&fontParams, 0.0f);
    C2D_SpriteSetScale(&fontParams, 1.0f, 1.0f);
    this->width = node->IntAttribute("width", 1);
    this->height = node->IntAttribute("height", 1);
    this->blink = 0;
    this->cursorDraw = true;

    // Save callbacks
    const char *onUpdateStr = node->Attribute("onUpdate");
    if(onUpdateStr) onUpdate = std::string(onUpdateStr);

    // Save controller
    this->controller = controller;

    // Open the terminal
    vt = tmt_open(height, width, NULL, NULL, NULL);
    if (vt == NULL) {
        throw std::runtime_error("Couldn't open terminal");
    }
}

/**
 * @brief Draw a console view
 */
void ConsoleView::draw() {

    if(this->controller != nullptr && !onUpdate.empty()) {
        ConsoleUpdateParams params;
        params.text = "";
        params.update = false;
        params.controller = controller;
        controller->callMethod(onUpdate, &params);

        if(params.update) {
            tmt_write(vt, params.text.c_str(), 0);
        }
    }

    float x = fontParams.params.pos.x;
    float y = fontParams.params.pos.y;

    const TMTSCREEN *screen = tmt_screen(vt);

    for(u16 i = 0; i < height; i++) {
        for(u16 j = 0; j < width; j++) {
            fontParams.params.pos.x = x + j * FONT_SIZE;
            fontParams.params.pos.y = y + i * FONT_SIZE;
            fontParams.image = C2D_SpriteSheetGetImage(fontData, screen->lines[i]->chars[j].c);
            C2D_DrawSprite(&fontParams);
        }
    }

    fontParams.params.pos.x = x;
    fontParams.params.pos.y = y;

    if(cursorDraw) {
        const TMTPOINT *c = tmt_cursor(vt);
        C2D_DrawRectSolid(x + c->c * FONT_SIZE - FONT_SIZE/2.0f, y + c->r * FONT_SIZE - FONT_SIZE/2.0f, 0.0f, FONT_SIZE, FONT_SIZE, C2D_Color32(0xFF, 0xFF, 0xFF, 0xFF));
    }

    if(blink < BLINK_SPEED) {
        blink ++;
    } else {
        blink = 0;
        cursorDraw = !cursorDraw;
    }
}

/**
 * @brief Destructor for a console view
 */
ConsoleView::~ConsoleView() {
    tmt_close(vt);
}

}
