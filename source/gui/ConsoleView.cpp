/**
 * @file ConsoleView.cpp
 * @brief View for text consoles
 */

// Includes TMT
#include <tmt.h>

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
    C2D_SpriteSetPos(&fontParams, 0.0f, 0.0f);
    C2D_SpriteSetRotation(&fontParams, 0.0f);
    C2D_SpriteSetScale(&fontParams, 1.0f, 1.0f);

    // Save controller
    this->controller = controller;
}

/**
 * @brief Process input
 */
void ConsoleView::input(u32 held, u32 down, u32 up, touchPosition &touch) {

}

/**
 * @brief Draw a console view
 */
void ConsoleView::draw() {

}

/**
 * @brief Destructor for a console view
 */
ConsoleView::~ConsoleView() { }

}
