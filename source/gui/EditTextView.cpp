/**
 * @file EditTextView.cpp
 * @brief View for editable text
 */

// Includes
#include <stdexcept>

// Own includes
#include "gui/EditTextView.h"
#include "Application.h"

using namespace tinyxml2;

namespace NetMan {

/**
 * @brief Create an edit text view
 * @param x             Text box x position
 * @param y             Text box y position
 * @param width         Text box width
 * @param height        Text box height
 * @param textLength    Maximum text length to be shown (actual text can be larger)
 * @param controller    Controller used for the view
 * @param editMethod    Method name when this text box is edited
 */
EditTextView::EditTextView(float x, float y, float width, float height, u16 textLength, std::shared_ptr<GuiController> controller, const std::string &editMethod) {

     // Get box parameters
    this->x = x;
    this->y = y;
    this->width = width;
    this->height = height;
    this->textColor = C2D_Color32(0, 0, 0, 0xFF);
    this->textScale = height / DEFAULT_FONT_SIZE;
    this->numeric = false;
    this->password = false;
    onEdit = editMethod;

    // Create text
    this->textLength = textLength;
    textBuffer = C2D_TextBufNew(textLength);
    if(textBuffer == NULL) {
        throw std::runtime_error("C2D_TextBufNew");
    }
	C2D_TextParse(&text, textBuffer, "");
	C2D_TextOptimize(&text);

    // Save controller
    this->controller = controller;

    // Call controller to initialize the field
    if(controller != nullptr && !onEdit.empty()) {
        char textStr[128];
        EditTextParams params;
        params.controller = controller;
        params.init = false;
        params.text = textStr;
        controller->callMethod(onEdit, &params);
        if(params.init) {
            C2D_TextParse(&text, textBuffer, textStr);
            C2D_TextOptimize(&text);
        }
    }
}

/**
 * @brief Load an edit text view
 * @param node          XML node containing image parameters
 * @param controller    Controller used (can be nullptr)
 */
EditTextView::EditTextView(XMLElement *node, std::shared_ptr<GuiController> controller) {

    // Get box parameters
    this->x = node->FloatAttribute("x", 0.0f);
    this->y = node->FloatAttribute("y", 0.0f);
    this->width = node->FloatAttribute("width", 50.0f);
    this->height = node->FloatAttribute("height", 50.0f);
    this->textColor = C2D_Color32(node->IntAttribute("r", 0), node->IntAttribute("g", 0), node->IntAttribute("b", 0), 0xFF);
    this->textScale = height / DEFAULT_FONT_SIZE;
    this->numeric = node->BoolAttribute("numeric", false);
    this->password = node->BoolAttribute("password", false);
    
    const char *onEditStr = node->Attribute("onEdit");
    if(onEditStr != NULL) {
        onEdit = std::string(onEditStr);
    }

    const char *hintTextStr = node->Attribute("hintText");
    if(hintTextStr != NULL) {
        hintText = std::string(hintTextStr);
    }

    // Create text
    textLength = node->IntAttribute("length", width / (DEFAULT_FONT_SIZE * textScale));
    textBuffer = C2D_TextBufNew(textLength);
    if(textBuffer == NULL) {
        throw std::runtime_error("C2D_TextBufNew");
    }
	C2D_TextParse(&text, textBuffer, "");
	C2D_TextOptimize(&text);

    // Save controller
    this->controller = controller;

    // Call controller to initialize the field
    if(controller != nullptr && !onEdit.empty()) {
        char textStr[128];
        EditTextParams params;
        params.controller = controller;
        params.init = false;
        params.text = textStr;
        controller->callMethod(onEdit, &params);
        if(params.init) {
            C2D_TextParse(&text, textBuffer, textStr);
            C2D_TextOptimize(&text);
        }
    }
}

/**
 * @brief Process input
 */
void EditTextView::input(u32 held, u32 down, u32 up, touchPosition &touch) {

    if(touch.px >= x && touch.px <= (x + width) &&
       touch.py >= y && touch.py <= (y + height) && (down &KEY_TOUCH)) {
        SwkbdState swkbd;
        char textstr[128];
        if(numeric) {
            swkbdInit(&swkbd, SWKBD_TYPE_NUMPAD, 1, 5);     // 5 digits for ports
			swkbdSetValidation(&swkbd, SWKBD_ANYTHING, 0, 0);
			swkbdSetFeatures(&swkbd, SWKBD_FIXED_WIDTH);
			swkbdSetNumpadKeys(&swkbd, 0, 0);
			swkbdInputText(&swkbd, textstr, sizeof(textstr));
        } else {
            swkbdInit(&swkbd, SWKBD_TYPE_WESTERN, 1, 128);
            if(password) swkbdSetPasswordMode(&swkbd, SWKBD_PASSWORD_HIDE_DELAY);
            swkbdSetValidation(&swkbd, SWKBD_NOTEMPTY_NOTBLANK, SWKBD_FILTER_AT | SWKBD_FILTER_PERCENT | SWKBD_FILTER_BACKSLASH | SWKBD_FILTER_PROFANITY, 0);
            swkbdSetFeatures(&swkbd, 0);
            swkbdSetHintText(&swkbd, hintText.c_str());
            swkbdInputText(&swkbd, textstr, sizeof(textstr));
        }

        // Call controller callback
        EditTextParams params;
        params.init = true;
        params.text = textstr;
        params.controller = controller;
        if(controller != nullptr && !onEdit.empty()) {
            controller->callMethod(onEdit, &params);
        }

        if(params.init) {
            C2D_TextBufClear(textBuffer);
            if(password) {
                memset(textstr, '*', strlen(textstr));
            }
            C2D_TextParse(&text, textBuffer, textstr);
            C2D_TextOptimize(&text);
        }
    }
}

/**
 * @brief Draw an image view
 */
void EditTextView::draw() {
    C2D_DrawRectSolid(x, y, 0.0f, width, height, C2D_Color32(240, 240, 160, 0xFF));
    C2D_DrawText(&text, C2D_WithColor, x, y, 0, textScale, textScale, textColor);
}

/**
 * @brief Destructor for an image view
 */
EditTextView::~EditTextView() {
    C2D_TextBufDelete(textBuffer);
}

}
