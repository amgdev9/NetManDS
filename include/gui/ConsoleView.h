/**
 * @file ConsoleView.h
 * @brief View for text consoles
 */

#ifndef _CONSOLE_VIEW_H_
#define _CONSOLE_VIEW_H_

// Includes C/C++
#include <memory>

// Includes 3DS
#include <citro2d.h>

// Includes tinyxml2
#include <tinyxml2.h>

// Own includes
#include "GuiView.h"
#include "controller/GuiController.h"
#include "ssh/tmt.h"

// Defines
#define FONT_SIZE   8.0f
#define BLINK_SPEED 30

namespace NetMan {

/**
 * @struct ConsoleUpdateParams
 */
typedef struct {
    std::shared_ptr<GuiController> controller;
    std::string text;
    bool update;
} ConsoleUpdateParams;

/**
 * @class ConsoleView
 */
class ConsoleView : public GuiView {
    private:
        C2D_SpriteSheet fontData;
        C2D_Sprite fontParams;
        std::shared_ptr<GuiController> controller;
        u16 width;
        u16 height;
        std::string onUpdate;
        TMT *vt;
        u8 blink;
        bool cursorDraw;
    public:
        ConsoleView(tinyxml2::XMLElement *node, std::shared_ptr<GuiController> controller);
        void draw() override;
        virtual ~ConsoleView();
};

}

#endif
