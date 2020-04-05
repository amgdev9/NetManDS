/**
 * @file TextView.cpp
 * @brief View for text
 */

// Includes
#include <stdexcept>

// Own includes
#include "gui/TextView.h"

using namespace tinyxml2;

namespace NetMan {

/**
 * @brief Load a text view
 * @param node          XML node containing image parameters
 * @param controller    Controller used (can be nullptr)
 */
TextView::TextView(XMLElement *node, std::shared_ptr<GuiController> controller) {

    // Get text
    const char *textValue = node->Attribute("text");
    if(textValue == NULL) {
        throw std::runtime_error("No text");
    }

    // Create text
    textBuffer = C2D_TextBufNew(strlen(textValue) + 1);
    if(textBuffer == NULL) {
        throw std::runtime_error("C2D_TextBufNew");
    }
	C2D_TextParse(&text, textBuffer, textValue);
	C2D_TextOptimize(&text);

    // Get text parameters
    x = node->FloatAttribute("x", 0.0f);
    y = node->FloatAttribute("y", 0.0f);
    scale = node->FloatAttribute("size", 1.0f);
    color = C2D_Color32(node->IntAttribute("r", 0), node->IntAttribute("g", 0), node->IntAttribute("b", 0), 0xFF);

    // Save controller
    this->controller = controller;
}

/**
 * @brief Create a text view
 * @param text      Text to use
 * @param textView  Text view to copy position, color...
 */
TextView::TextView(const std::string &textValue, std::shared_ptr<TextView> textView) {

    // Create text
    textBuffer = C2D_TextBufNew(textValue.length() + 1);
    if(textBuffer == NULL) {
        throw std::runtime_error("C2D_TextBufNew");
    }
	C2D_TextParse(&text, textBuffer, textValue.c_str());
	C2D_TextOptimize(&text);

    // Get text parameters
    x = textView->x;
    y = textView->y;
    scale = textView->scale;
    color = textView->color;

    // Save controller
    this->controller = controller;
}

/**
 * @brief Draw a text view
 */
void TextView::draw() {
    C2D_DrawText(&text, C2D_WithColor, x, y, 0, scale, scale, color);
}

/**
 * @brief Destructor for an image view
 */
TextView::~TextView() {
    C2D_TextBufDelete(textBuffer);
}

}
